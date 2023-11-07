// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "CharacterAnimInstance.h"
#include "KamikazeStrikeforce/PlayerController/MainPlayerController.h"
#include "KamikazeStrikeforce/PlayerState/MainPlayerState.h"
#include "KamikazeStrikeforce/HUD/MainHUD.h"
#include "KamikazeStrikeforce/GameMode/KamikazeStrikeforceGameMode.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "KamikazeStrikeforce/Weapon/Weapon.h"
#include "KamikazeStrikeforce/Components/CombatComponent.h"
#include "KamikazeStrikeforce/Components/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

///////////////////////////////////////////////////////////a///////////////
// AMainCharacter

AMainCharacter::AMainCharacter()
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

	lagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("Lag Compensation Component"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_GameTraceChannel1);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	dissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve Timeline Component"));

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	turnInPlace = ETurnInPlace::None;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	inputContextNotSet = true;

	//Hit Box Init...
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("hand_r"), hand_r);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitCollisionBoxes.Add(FName("foot_r"), foot_r);
}


void AMainCharacter::PollInitAvatar()
{	
	if (currentAvatar != EAvatar::None) return;
	if(HasAuthority())
	{
		AMainPlayerController* cont = Cast<AMainPlayerController>(GetController());
		if (cont)
		{
			currentAvatar = cont->playerAvatar;
			InitAvatar(currentAvatar);
		}
	}
}

void AMainCharacter::OnRep_Avatar()
{
	InitAvatar(currentAvatar);
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, overlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AMainCharacter, health);
	DOREPLIFETIME(AMainCharacter, disableGameplay);
	DOREPLIFETIME(AMainCharacter, currentAvatar);
}

void AMainCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	playerController = Cast<AMainPlayerController>(Controller);

	//Add Input Mapping Context
	if (playerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		mainHUD = Cast<AMainHUD>(playerController->GetHUD());
		playerController->SetHUDHealth(health, maxHealth);
	}

	animInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AMainCharacter::ReceiveDamage);
	}
			
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (combat)
	{
		combat->character = this; 
	}
	if (lagCompensationComponent)
	{
		lagCompensationComponent->character = this;
		if (playerController)
			lagCompensationComponent->playerController = playerController;
	}
}

void AMainCharacter::PollInitializePlayerState()
{
	if (!mainPlayerState) mainPlayerState = Cast<AMainPlayerState>(GetPlayerState());
	if (mainPlayerState && !playerStateSet)
	{
		mainPlayerState->AddScore(0.0f);
		mainPlayerState->AddDeaths(0);
		playerStateSet = true;
	}
}


void AMainCharacter::InitAvatar(EAvatar avatar)
{
	switch (avatar)
	{
	case EAvatar::Manny:
		if (mannyMesh)
			GetMesh()->SetSkeletalMesh(mannyMesh);
		break;

	case EAvatar::Quinn:
		if (quinnMesh)
			GetMesh()->SetSkeletalMesh(quinnMesh);
		break;

	case EAvatar::Hazard:
		if (hazardMesh)
			GetMesh()->SetSkeletalMesh(hazardMesh);
		break;

	case EAvatar::Grind:
		if (grindMesh)
			GetMesh()->SetSkeletalMesh(grindMesh);
		break;

	default:
		break;
	}
}



void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if (!disableGameplay)
	{
		AimOffset(DeltaTime);
	}
	else
	{
		turnInPlace = ETurnInPlace::None;
		bUseControllerRotationYaw = false;
	}
	CheckInputContext();

	if(IsLocallyControlled())
		HideCamIfCharClose();

	PollInitializePlayerState();
	PollInitAvatar();

	/*if (HasAuthority())
	{
		FString str = FString::Printf(TEXT("%d: avatar: %d"), (int)GPlayInEditorID, (int)currentAvatar);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, str);
	}*/
}



//////////////////////////////////////////////////////////////////////////
// Input

void AMainCharacter::CheckInputContext()
{
	if (inputContextNotSet && playerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			if (!Subsystem->HasMappingContext(DefaultMappingContext))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Cyan, FString::Printf(TEXT("Mapping set")));
				inputContextNotSet = false;
			}
			else
				inputContextNotSet = false;
		}
	}
}



void AMainCharacter::AimPressed()
{
	if (disableGameplay) return;

	if (combat)
	{
		combat->SetAiming(true);
	}
}

void AMainCharacter::AimReleased()
{
	if (disableGameplay) return;

	if (combat)
	{
		combat->SetAiming(false);
	}
}

void AMainCharacter::FirePressed()
{
	if (disableGameplay) return;

	if (combat)
	{
		combat->FirePressed(true);
	}
}

void AMainCharacter::FireReleased()
{
	if (disableGameplay) return;

	if (combat)
	{
		combat->FirePressed(false);
	}
}


void AMainCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	/*SimProxiesTurn();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan, FString::Printf(TEXT("Turn = %d"), turnInPlace));

	lastRepMovementUpdateTime = 0;*/
}


void AMainCharacter::SimProxiesTurn()
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

void AMainCharacter::AimOffset(float deltaTime)
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

void AMainCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		AO_Pitch = FMath::GetMappedRangeValueClamped(FVector2D(270.0f, 360.0f), FVector2D(-90.0f, 0.0f), AO_Pitch);
	}
	//if GEngine)
		//ngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Cyan, FString::Printf(TEXT("AO Yaw = %f"), AO_Yaw));
}

void AMainCharacter::UpdateTurnInPlace(float deltaTime)
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



void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMainCharacter::JumpPressed);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMainCharacter::JumpReleased);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AMainCharacter::EquipPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMainCharacter::CrouchPressed);


		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMainCharacter::AimPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMainCharacter::AimReleased);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMainCharacter::FirePressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AMainCharacter::FireReleased);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMainCharacter::ReloadPressed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (disableGameplay) return;

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

void AMainCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		float aimSensitivity = 1.0f;
		if (IsAiming() && combat && combat->equippedWeapon)
			aimSensitivity = combat->equippedWeapon->GetAimSensitivity();
		
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * aimSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * aimSensitivity);
	}
}

void AMainCharacter::JumpPressed()
{
	if (disableGameplay) return;
	Jump();
}

void AMainCharacter::JumpReleased()
{
	if (disableGameplay) return;
	StopJumping();
}

void AMainCharacter::EquipPressed()
{
	if (disableGameplay) return;

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

void AMainCharacter::ServerEquipPressed_Implementation()
{
	if (combat && overlappingWeapon)
	{
		combat->EquipWeapon(overlappingWeapon);
	}
}


void AMainCharacter::CrouchPressed()
{
	if (disableGameplay) return;

	if (!bIsCrouched)
		Crouch();
	else
		UnCrouch();
}


void AMainCharacter::ReloadPressed()
{
	if (disableGameplay) return;

	if (combat) combat->Reload();
}


void AMainCharacter::HideCamIfCharClose()
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



void AMainCharacter::SetOverlappingWeapon(AWeapon* weapon)
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


void AMainCharacter::OnRep_OverlappingWeapon(AWeapon* lastWeapon)
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

bool AMainCharacter::IsEquipped()
{
	return (combat && combat->equippedWeapon);
}

bool AMainCharacter::IsAiming()
{
	return (combat && combat->isAiming);
}

AMainHUD* AMainCharacter::GetMainHUD()
{
	if (!mainHUD)
	{
		if (!playerController) 
			playerController = Cast<AMainPlayerController>(Controller);

		if(playerController)
			mainHUD = Cast<AMainHUD>(playerController->GetHUD());
	}
	return mainHUD;
}

bool AMainCharacter::IsLocallyReloading() const
{
	return (combat && combat->isLocallyReloading);
}

AMainPlayerController* AMainCharacter::GetPlayerController()
{
	if(!playerController) playerController = Cast<AMainPlayerController>(Controller);

	return playerController; 
}

ECombatState AMainCharacter::GetCombatState() const
{
	return combat->combatState; 
}

FVector AMainCharacter::GetHitLocation()
{
	 return combat->GetHitLocation(); 
}


AWeapon* AMainCharacter::GetEquippedWeapon()
{
	if (combat)
		return combat->equippedWeapon;
	else
		return nullptr;
}

void AMainCharacter::PlayFireMontage(bool isAiming)
{
	if (animInstance && fireMontage)
	{
		animInstance->Montage_Play(fireMontage);
		FName section = isAiming ? FName("FireAim") : FName("FireHip");
		if(combat && combat->equippedWeapon && combat->equippedWeapon->GetWeaponType() == EWeaponTypes::Shotgun)
			section = isAiming ? FName("ShotgunAim") : FName("ShotgunHip");
		animInstance->Montage_JumpToSection(section);
	}
}

void AMainCharacter::PlayReloadMontage()
{
	if (animInstance && reloadMontage && GetEquippedWeapon())
	{
		animInstance->Montage_Play(reloadMontage);
		FName section;
		switch (GetEquippedWeapon()->GetWeaponType())
		{
			case EWeaponTypes::AssaultRifle:
				section = FName("Rifle");
				break;

			case EWeaponTypes::RocketLauncher:
				section = FName("Rifle");
				break;

			case EWeaponTypes::Pistol:
				section = FName("Pistol");
				break;

			case EWeaponTypes::SMG:
				section = FName("Pistol");
				break;

			case EWeaponTypes::Shotgun:
				section = FName("Shotgun");
				break;

			default:
				section = FName("Rifle");
				break;
		}
		animInstance->Montage_JumpToSection(section);
	}
}

void AMainCharacter::PlayHitReactMontage()
{
	if (animInstance && hitReactMontage && !animInstance->Montage_IsPlaying(hitReactMontage))
	{
		animInstance->Montage_Play(hitReactMontage);
		FName section("FromFront");
		animInstance->Montage_JumpToSection(section);
	}
}

void AMainCharacter::OnRep_Health()
{
	UpdatePlayerHUDHealth();
	PlayHitReactMontage();
}

void AMainCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (isEliminated) return;

	health = FMath::Clamp(health - Damage, 0.0f, maxHealth);
	PlayHitReactMontage();
	UpdatePlayerHUDHealth();

	if (health == 0.0f)
	{
		AKamikazeStrikeforceGameMode* gameMode = GetWorld()->GetAuthGameMode<AKamikazeStrikeforceGameMode>();
		if (gameMode)
		{
			AMainPlayerController* attackerController = Cast<AMainPlayerController>(InstigatedBy);
			gameMode->PlayerEliminated(this, playerController, attackerController);
		}
	}
}

void AMainCharacter::UpdatePlayerHUDHealth()
{
	if(!playerController) 
		playerController = Cast<AMainPlayerController>(Controller);

	if (playerController)
		playerController->SetHUDHealth(health, maxHealth);
}

void AMainCharacter::PlayEliminationMontage()
{
	if (animInstance && eliminationMontage)
	{
		animInstance->Montage_Play(eliminationMontage);
	}
}

void AMainCharacter::Eliminate()
{
	if (combat) combat->DropWeapon();
	MulticastEliminate();
	GetWorld()->GetTimerManager().SetTimer(eliminationTimer, this, &AMainCharacter::EliminationFinished, eliminiationDelay);
}

UCombatComponent* AMainCharacter::GetCombat() const
{
	return combat;
}

void AMainCharacter::MulticastEliminate_Implementation()
{
	isEliminated = true;

	if (dissolveMaterialInstances.Num() > 0)
	{
		for (int i = 0; i < dissolveMaterialInstances.Num(); i++)
		{
			auto inst = UMaterialInstanceDynamic::Create(dissolveMaterialInstances[i], this);
			dynamicDissolveMaterialInstances.Add(inst);
			GetMesh()->SetMaterial(i, inst);
			inst->SetScalarParameterValue(TEXT("Dissolve"), startDissolveValue);
			inst->SetScalarParameterValue(TEXT("Glow"), 200.f);
		}
		StartDissolve();
	}

	disableGameplay = true;

	if (combat)combat->FirePressed(false);

	if (playerController)
	{
		playerController->SetHUDAmmo(0);
		playerController->SetHUDCarriedAmmo(0);
	}

	// Disable collision
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	if (IsLocallyControlled() && combat && combat->equippedWeapon && combat->equippedWeapon->GetWeaponType() == EWeaponTypes::Sniper)
	{
		ShowSniperScope(false);
	}
}


void AMainCharacter::EliminationFinished()
{
	AKamikazeStrikeforceGameMode* gameMode = GetWorld()->GetAuthGameMode<AKamikazeStrikeforceGameMode>();
	if (gameMode)
	{
		gameMode->RequestRespawn(this, playerController);
	}
}

void AMainCharacter::StartDissolve()
{
	dissolveTrack.BindDynamic(this, &AMainCharacter::UpdateDissolveMaterial);
	if (dissolveCurve && dissolveTimeline)
	{
		dissolveTimeline->AddInterpFloat(dissolveCurve, dissolveTrack);
		dissolveTimeline->Play();
	}
}

void AMainCharacter::UpdateDissolveMaterial(float dissolveValue)
{
	if (dynamicDissolveMaterialInstances.Num() > 0)
	{
		for (int i = 0; i < dynamicDissolveMaterialInstances.Num(); i++)
		{
			dynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Dissolve"), dissolveValue);
		}
	}
}

void AMainCharacter::Destroyed()
{
	AKamikazeStrikeforceGameMode* gameMode = Cast<AKamikazeStrikeforceGameMode>(UGameplayStatics::GetGameMode(this));
	if (combat && combat->equippedWeapon && gameMode && gameMode->GetMatchState() != MatchState::InProgress) 
		combat->equippedWeapon->Destroy();
	Super::Destroyed();
}