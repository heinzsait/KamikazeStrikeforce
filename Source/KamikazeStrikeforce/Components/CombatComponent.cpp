// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "KamikazeStrikeforce/Weapon/Weapon.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/PlayerController/MainPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	baseWalkSpeed = 600;
	aimWalkSpeed = 400;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, equippedWeapon);
	DOREPLIFETIME(UCombatComponent, isAiming);
	DOREPLIFETIME(UCombatComponent, combatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, carriedAmmo, COND_OwnerOnly);
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	/*if (!character)
	{
		character = Cast<AMainCharacter>(GetOwner());
	}*/
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
		defaultFOV = character->GetCamera()->FieldOfView;
		currentFOV = character->GetCamera()->FieldOfView;
	}	

	canFire = true;

	if(character->HasAuthority())
		InitAmmos();
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (character && character->IsLocallyControlled())
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Combat Update")));

		TraceCrosshair(hitResult);
		LerpFOV(DeltaTime);
		SetHUDCrosshair(DeltaTime);
	}

}

void UCombatComponent::EquipWeapon(AWeapon* weapon)
{	
	if (character && weapon)
	{
		if (equippedWeapon)
			equippedWeapon->DropWeapon();

		const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handSocket)
		{
			equippedWeapon = weapon;
			equippedWeapon->SetWeaponState(EWeaponState::Equipped);
			equippedWeapon->SetOwner(character);

			equippedWeapon->UpdateHUDAmmo();
			if (carriedAmmoMap.Contains(equippedWeapon->GetWeaponType()))
			{
				carriedAmmo = carriedAmmoMap[equippedWeapon->GetWeaponType()];
				if (character->GetPlayerController())
					character->GetPlayerController()->SetHUDCarriedAmmo(carriedAmmo);
			}			

			handSocket->AttachActor(equippedWeapon, character->GetMesh());

			if (equippedWeapon && equippedWeapon->equipSFX)
				UGameplayStatics::PlaySoundAtLocation(this, equippedWeapon->equipSFX, character->GetActorLocation());

			if (equippedWeapon && equippedWeapon->IsEmpty()) Reload();

			character->GetCharacterMovement()->bOrientRotationToMovement = false;
			character->bUseControllerRotationYaw = true;
		}
	}
}


void UCombatComponent::OnRep_EquippedWeapon()
{
	if (equippedWeapon && character)
	{		
		const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handSocket)
		{	
			equippedWeapon->SetWeaponState(EWeaponState::Equipped);		
			handSocket->AttachActor(equippedWeapon, character->GetMesh());

			if (equippedWeapon && equippedWeapon->equipSFX)
				UGameplayStatics::PlaySoundAtLocation(this, equippedWeapon->equipSFX, character->GetActorLocation());

			character->GetCharacterMovement()->bOrientRotationToMovement = false;
			character->bUseControllerRotationYaw = true;
		}
	}
}

void UCombatComponent::Reload()
{
	if (carriedAmmo > 0 && combatState != ECombatState::Reloading && equippedWeapon && (equippedWeapon->GetRoomInMag() > 0))
	{
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (equippedWeapon)
	{
		combatState = ECombatState::Reloading;		
		HandleReload();
	}
}

void UCombatComponent::ReloadWeaponAmmo()
{
	if (equippedWeapon)
	{
		int reloadAmount = AmountToReload();
		if (carriedAmmoMap.Contains(equippedWeapon->GetWeaponType()))
		{
			carriedAmmoMap[equippedWeapon->GetWeaponType()] -= reloadAmount;
			carriedAmmo = carriedAmmoMap[equippedWeapon->GetWeaponType()];
			if (character->GetPlayerController())
				character->GetPlayerController()->SetHUDCarriedAmmo(carriedAmmo);
		}
		equippedWeapon->AddAmmo(reloadAmount);
	}
}


void UCombatComponent::HandleReload()
{
	if (character)
	{
		character->PlayReloadMontage();
	}
}
int UCombatComponent::AmountToReload()
{
	int amt = 0;
	if (equippedWeapon)
	{		
		if (carriedAmmoMap.Contains(equippedWeapon->GetWeaponType()))
		{
			int roomInMag = equippedWeapon->GetRoomInMag();
			int min = FMath::Min(roomInMag, carriedAmmoMap[equippedWeapon->GetWeaponType()]);
			amt = FMath::Clamp(roomInMag, 0, min);
		}
	}
	return amt;
}


void UCombatComponent::OnRep_CombatState()
{
	switch (combatState)
	{
	case ECombatState::Unoccupied:
		if (isFirePressed)
			Fire();
		break;

	case ECombatState::Reloading:
		HandleReload();
		break;

	case ECombatState::MAX:
		break;

	default:
		break;
	}
}

void UCombatComponent::FinishReloading()
{
	if (character->HasAuthority())
	{
		combatState = ECombatState::Unoccupied;
		ReloadWeaponAmmo();
	}

	if (isFirePressed)
		Fire();
}

void UCombatComponent::LerpFOV(float DeltaTime)
{
	if (equippedWeapon)
	{
		if (isAiming)
		{
			currentFOV = FMath::FInterpTo(currentFOV, equippedWeapon->GetZoomFOV(), DeltaTime, equippedWeapon->GetZoomSpeed());
		}
		else
		{
			currentFOV = FMath::FInterpTo(currentFOV, defaultFOV, DeltaTime, zoomSpeed);
		}

		if (character && character->GetCamera())
		{
			character->GetCamera()->FieldOfView = currentFOV;
		}
	}
}



void UCombatComponent::ServerSetAiming_Implementation(bool _isAiming)
{
	isAiming = _isAiming;
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}

void UCombatComponent::SetAiming(bool _isAiming)
{
	isAiming = _isAiming;
	ServerSetAiming(_isAiming);
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}

void UCombatComponent::FirePressed(bool isPressed)
{
	isFirePressed = isPressed;

	if (isFirePressed)
		Fire();
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		ServerFire(hitLocation);
		if (equippedWeapon)
			crosshairShootFactor = 0.75f;
		StartFireTimer();
	}
}

bool UCombatComponent::CanFire()
{
	return (equippedWeapon && !equippedWeapon->IsEmpty() && canFire && combatState == ECombatState::Unoccupied);
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize hitTarget)
{
	MultiCastFire(hitTarget);
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize hitTarget)
{
	if (character && equippedWeapon && combatState == ECombatState::Unoccupied)
	{
		character->PlayFireMontage(isAiming);
		equippedWeapon->Fire(hitTarget);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (character && equippedWeapon)
	{
		canFire = false;
		character->GetWorldTimerManager().SetTimer(fireTimer, this, &UCombatComponent::FireTimerFinished, equippedWeapon->fireDelay);
	}
}

void UCombatComponent::FireTimerFinished()
{
	canFire = true;
	if (isFirePressed && equippedWeapon && equippedWeapon->isAutomatic)
	{
		Fire();
	}
	if (equippedWeapon && equippedWeapon->IsEmpty()) Reload();
}

void UCombatComponent::DropWeapon()
{ 
	if (equippedWeapon)
	{
		equippedWeapon->DropWeapon();
		equippedWeapon = nullptr;
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	if (character && character->GetPlayerController())
		character->GetPlayerController()->SetHUDCarriedAmmo(carriedAmmo);
}

void UCombatComponent::InitAmmos()
{
	carriedAmmoMap.Emplace(EWeaponTypes::AssaultRifle, startingARAmmo);
	carriedAmmoMap.Emplace(EWeaponTypes::RocketLauncher, startingRocketAmmo);
	carriedAmmoMap.Emplace(EWeaponTypes::Pistol, startingPistolAmmo);
}


void UCombatComponent::TraceCrosshair(FHitResult& result)
{
	FVector2D viewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewPortSize);
	}

	FVector2D crosshairLocation(viewPortSize.X / 2, viewPortSize.Y / 2);
	FVector crosshairWorldLocation;
	FVector crosshairWorldDirection;
	bool success = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), crosshairLocation, crosshairWorldLocation, crosshairWorldDirection);
	if (success && character && equippedWeapon)
	{
		FVector start = crosshairWorldLocation ;
		
		float dist = (character->GetActorLocation() - start).Size();
		start += crosshairWorldDirection * (dist + 20.f);

		FVector end = start + crosshairWorldDirection * 100000;

		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(character);
		queryParams.AddIgnoredActor(equippedWeapon);
		GetWorld()->LineTraceSingleByChannel(result, start, end, ECC_Visibility, queryParams);
		if (!result.bBlockingHit)
		{
			result.ImpactPoint = end;
		}

		hitLocation = result.ImpactPoint;

		//DrawDebugSphere(GetWorld(), hitLocation, 10, 10, FColor::Red);
		//DrawDebugLine(GetWorld(), start, hitLocation, FColor::Orange);

		//if (GEngine && result.GetActor())
			//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->DeltaTimeSeconds, FColor::Cyan, FString::Printf(TEXT("Hit Name: %s"), *result.GetActor()->GetFName().ToString()));

		if (result.GetActor() && Cast<ICrosshairHitInterface>(result.GetActor()))
		{
			HUDPackage.crosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.crosshairColor = FLinearColor::White;
		}
		
	}
}

void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{
	/*if (!character->GetPlayerController())
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Controller not valid")));
	}
	if (!character->GetMainHUD())
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("HUDs not valid")));
	}*/
	if (character && character->GetPlayerController() && character->GetMainHUD())
	{
		if (equippedWeapon)
		{
			HUDPackage.centerCH = equippedWeapon->centerCH;
			HUDPackage.bottomCH = equippedWeapon->bottomCH;
			HUDPackage.topCH = equippedWeapon->topCH;
			HUDPackage.rightCH = equippedWeapon->rightCH;
			HUDPackage.leftCH = equippedWeapon->leftCH;
		}
		else
		{
			HUDPackage.centerCH = nullptr;
			HUDPackage.bottomCH = nullptr;
			HUDPackage.topCH = nullptr;
			HUDPackage.rightCH = nullptr;
			HUDPackage.leftCH = nullptr;
		}

		// [0, 600] -> [0, 1]
		FVector2D WalkSpeedRange(0.f, character->GetCharacterMovement()->MaxWalkSpeed);
		FVector2D VelocityMultiplierRange(0.f, 1.f);
		FVector Velocity = character->GetVelocity();
		Velocity.Z = 0.f;

		crosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

		if (character->GetCharacterMovement()->IsFalling())
		{
			crosshairInAirFactor = FMath::FInterpTo(crosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
		}
		else
		{
			crosshairInAirFactor = FMath::FInterpTo(crosshairInAirFactor, 0.f, DeltaTime, 30.f);
		}

		if (equippedWeapon)
		{
			if (isAiming)
			{
				crosshairAimFactor = FMath::FInterpTo(crosshairAimFactor, 0.58f, DeltaTime, equippedWeapon->GetZoomSpeed());
			}
			else
			{
				crosshairAimFactor = FMath::FInterpTo(crosshairAimFactor, 0.f, DeltaTime, equippedWeapon->GetZoomSpeed());
			}
		}

		crosshairShootFactor = FMath::FInterpTo(crosshairShootFactor, 0.f, DeltaTime, 40.0f);

		HUDPackage.spreadSpeed = 0.5f + crosshairVelocityFactor + crosshairInAirFactor + crosshairShootFactor - crosshairAimFactor;

		character->GetMainHUD()->SetHUDPackage(HUDPackage);
	}
}



