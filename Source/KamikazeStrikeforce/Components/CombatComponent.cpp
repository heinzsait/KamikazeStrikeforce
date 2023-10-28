// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "KamikazeStrikeforce/Weapon/Weapon.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

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
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	}
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::EquipWeapon(AWeapon* weapon)
{	
	if (character && weapon)
	{
		const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handSocket)
		{
			equippedWeapon = weapon;
			equippedWeapon->SetWeaponState(EWeaponState::Equipped);
			equippedWeapon->SetOwner(character);
			handSocket->AttachActor(equippedWeapon, character->GetMesh());

			character->GetCharacterMovement()->bOrientRotationToMovement = false;
			character->bUseControllerRotationYaw = true;
		}
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (equippedWeapon && character)
	{
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
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
	{
		FHitResult hitResult;
		TraceCrosshair(hitResult);
		ServerFire(hitResult.ImpactPoint);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize hitTarget)
{
	MultiCastFire(hitTarget);
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize hitTarget)
{
	if (character && equippedWeapon)
	{
		character->PlayFireMontage(isAiming);
		equippedWeapon->Fire(hitTarget);
	}
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
	if (success)
	{
		FVector start = crosshairWorldLocation ;
		FVector end = start + crosshairWorldDirection * 100000;
		GetWorld()->LineTraceSingleByChannel( result, start, end, ECC_Visibility);
		if (!result.bBlockingHit)
		{
			result.ImpactPoint = end;
		}
	}
}

