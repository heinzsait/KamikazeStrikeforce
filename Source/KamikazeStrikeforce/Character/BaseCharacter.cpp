// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "BaseAnimInstance.h"
#include "KamikazeStrikeforce/PlayerController/BasePlayerController.h"
#include "KamikazeStrikeforce/HUD/BaseHUD.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "KamikazeStrikeforce/Weapon/Weapon.h"
#include "KamikazeStrikeforce/Components/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

///////////////////////////////////////////////////////////a///////////////
// ABaseCharacter

ABaseCharacter::ABaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	combat->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_GameTraceChannel1);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	turnInPlace = ETurnInPlace::None;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseCharacter, overlappingWeapon, COND_OwnerOnly);
}

void ABaseCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	playerController = Cast<ABasePlayerController>(Controller);

	//Add Input Mapping Context
	if (playerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		mainHUD = Cast<ABaseHUD>(playerController->GetHUD());
	}

	animInstance = Cast<UBaseAnimInstance>(GetMesh()->GetAnimInstance());
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (combat)
	{
		combat->character = this; 
	}
}


void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
	/*if (GetLocalRole() > ROLE_SimulatedProxy)
	{
		AimOffset(DeltaTime);
	}
	else
	{
		lastRepMovementUpdateTime += DeltaTime;
		if (lastRepMovementUpdateTime > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}*/
	if(IsLocallyControlled())
		HideCamIfCharClose();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABaseCharacter::AimPressed()
{
	if (combat)
	{
		combat->SetAiming(true);
	}
}

void ABaseCharacter::AimReleased()
{
	if (combat)
	{
		combat->SetAiming(false);
	}
}

void ABaseCharacter::FirePressed()
{
	if (combat)
	{
		combat->FirePressed(true);
	}
}

void ABaseCharacter::FireReleased()
{
	if (combat)
	{
		combat->FirePressed(false);
	}
}


void ABaseCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	/*SimProxiesTurn();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan, FString::Printf(TEXT("Turn = %d"), turnInPlace));

	lastRepMovementUpdateTime = 0;*/
}

void ABaseCharacter::SimProxiesTurn()
{
	if (combat && combat->equippedWeapon)
	{	
		rotateRootBone = false;
		float speed = UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity);
		bool isInAir = (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying());
		if (speed > 0.0f || isInAir)
		{
			turnInPlace = ETurnInPlace::None;
			return;
		}
				
		proxyPrevRot = proxyRot;
		proxyRot = GetActorRotation();
		proxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(proxyRot, proxyPrevRot).Yaw;


		float turnThreshold = .25f;

		//ProxyRotationLastFrame = ProxyRotation;
		//ProxyRotation = GetActorRotation();
		//ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

		UE_LOG(LogTemp, Warning, TEXT("ProxyYaw: %f"), proxyYaw);

		if (FMath::Abs(proxyYaw) > turnThreshold)
		{
			if (proxyYaw > turnThreshold)
			{
				turnInPlace = ETurnInPlace::Right;
			}
			else if (proxyYaw < -turnThreshold)
			{
				turnInPlace = ETurnInPlace::Left;
			}
			else
			{
				turnInPlace = ETurnInPlace::None;
			}
			return;
		}
		turnInPlace = ETurnInPlace::None;


	}
}

void ABaseCharacter::AimOffset(float deltaTime)
{
	if (combat && combat->equippedWeapon)
	{
		
		float speed = UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity);
		bool isInAir = (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying());

		if (speed == 0.0f && !isInAir)
		{	
			rotateRootBone = true;
			FRotator currentAimRot = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
			FRotator deltaAimRot = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRot, startAimRot);
			AO_Yaw = deltaAimRot.Yaw;
			
			bUseControllerRotationYaw = true;
			UpdateTurnInPlace(deltaTime);
		}
		if (speed > 0.0f || isInAir)
		{
			rotateRootBone = false;
			startAimRot = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
			AO_Yaw = 0.0f;
			bUseControllerRotationYaw = true;
			turnInPlace = ETurnInPlace::None;
		}

		CalculateAO_Pitch();

	}
}

void ABaseCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		AO_Pitch = FMath::GetMappedRangeValueClamped(FVector2D(270.0f, 360.0f), FVector2D(-90.0f, 0.0f), AO_Pitch);
	}
	//if GEngine)
		//ngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Cyan, FString::Printf(TEXT("AO Yaw = %f"), AO_Yaw));
}

void ABaseCharacter::UpdateTurnInPlace(float deltaTime)
{
	if (turnInPlace == ETurnInPlace::None)
	{
		interpAO_Yaw = AO_Yaw;
	}

	if (AO_Yaw > 90.0f)
	{
		turnInPlace = ETurnInPlace::Right;
	}
	else if (AO_Yaw < -90.0f)
	{
		turnInPlace = ETurnInPlace::Left;
	}

	if (turnInPlace != ETurnInPlace::None)
	{	
		if (turnInPlace == ETurnInPlace::Right)
		{
			interpAO_Yaw = FMath::FInterpTo(interpAO_Yaw, 0.0f, deltaTime, 5.0f);
			AO_Yaw = interpAO_Yaw;
			if (FMath::Abs(AO_Yaw) < 15.0f)
			{
				turnInPlace = ETurnInPlace::None;
				startAimRot = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
			}
		}
		else if (turnInPlace == ETurnInPlace::Left)
		{
			interpAO_Yaw = FMath::FInterpTo(interpAO_Yaw, 0.0f, deltaTime, 3.0f);
			AO_Yaw = interpAO_Yaw;
			if (FMath::Abs(AO_Yaw) < 17.0f)
			{
				turnInPlace = ETurnInPlace::None;
				startAimRot = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
			}
		}
	}
	
}



void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ABaseCharacter::EquipPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABaseCharacter::CrouchPressed);


		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABaseCharacter::AimPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABaseCharacter::AimReleased);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABaseCharacter::AimPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABaseCharacter::AimReleased);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABaseCharacter::FirePressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABaseCharacter::FireReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABaseCharacter::EquipPressed()
{
	if (combat && overlappingWeapon)
	{
		if (HasAuthority())
		{
			combat->EquipWeapon(overlappingWeapon);
		}
		else
			ServerEquipPressed();
	}
}

void ABaseCharacter::ServerEquipPressed_Implementation()
{
	if (combat && overlappingWeapon)
	{
		combat->EquipWeapon(overlappingWeapon);
	}
}


void ABaseCharacter::CrouchPressed()
{
	if (!bIsCrouched)
		Crouch();
	else
		UnCrouch();
}

void ABaseCharacter::HideCamIfCharClose()
{
	if (IsLocallyControlled() && FVector::Distance(FollowCamera->GetComponentLocation(), GetActorLocation()) < camHideThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (combat && combat->equippedWeapon && combat->equippedWeapon->GetWeaponMesh())
		{
			combat->equippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (combat && combat->equippedWeapon && combat->equippedWeapon->GetWeaponMesh())
		{
			combat->equippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}


void ABaseCharacter::SetOverlappingWeapon(AWeapon* weapon)
{

	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString(TEXT("overlapping...")));

	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(false);
	}
	overlappingWeapon = weapon;
	if (overlappingWeapon)
	{
		if (IsLocallyControlled())
		{
			overlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABaseCharacter::OnRep_OverlappingWeapon(AWeapon* lastWeapon)
{

	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString(TEXT("overlapped in server")));
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(true);
	}
	if (lastWeapon)
	{
		lastWeapon->ShowPickupWidget(false);
	}
}

bool ABaseCharacter::IsEquipped()
{
	return (combat && combat->equippedWeapon);
}

bool ABaseCharacter::IsAiming()
{
	return (combat && combat->isAiming);
}

FVector ABaseCharacter::GetHitLocation()
{
	 return combat->GetHitLocation(); 
}


AWeapon* ABaseCharacter::GetEquippedWeapon()
{
	if (combat)
		return combat->equippedWeapon;
	else
		return nullptr;
}

void ABaseCharacter::PlayFireMontage(bool isAiming)
{
	if (animInstance && fireMontage)
	{
		animInstance->Montage_Play(fireMontage);
		FName section = isAiming ? FName("FireAim") : FName("FireHip");
		animInstance->Montage_JumpToSection(section);
	}
}

void ABaseCharacter::MulticastHitReact_Implementation()
{
	PlayHitReactMontage();
}

void ABaseCharacter::PlayHitReactMontage()
{
	if (animInstance && hitReactMontage)
	{
		animInstance->Montage_Play(hitReactMontage);
		FName section("FromFront");
		animInstance->Montage_JumpToSection(section);
	}
}

