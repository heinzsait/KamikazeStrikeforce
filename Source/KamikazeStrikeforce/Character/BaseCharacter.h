// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "KamikazeStrikeforce/Interfaces/CrosshairHitInterface.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCombatComponent;
class ABasePlayerController;
class ABaseHUD;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class ABaseCharacter : public ACharacter, public ICrosshairHitInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	
	UCombatComponent* combat;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Equip Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void EquipPressed();
	void CrouchPressed();

	UFUNCTION(Server, Reliable)
	void ServerEquipPressed();


	void AimPressed();
	void AimReleased();

	void FirePressed();
	void FireReleased();

	void AimOffset(float deltaTime);

	void UpdateTurnInPlace(float deltaTime);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void PostInitializeComponents() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }

	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	FORCEINLINE ETurnInPlace GetTurnInPlace() const { return turnInPlace; }

	FORCEINLINE ABasePlayerController* GetController() const { return playerController; }

	FORCEINLINE ABaseHUD* GetMainHUD() const { return mainHUD; }

	FORCEINLINE UCameraComponent* GetCamera() const { return FollowCamera; }

	FVector GetHitLocation();

private:

	ABasePlayerController* playerController;
	ABaseHUD* mainHUD;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* overlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* lastWeapon);

	float AO_Yaw;
	float AO_Pitch;
	float interpAO_Yaw;
	FRotator startAimRot;

	ETurnInPlace turnInPlace;

	class UBaseAnimInstance* animInstance;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* fireMontage;

	void HideCamIfCharClose();

	UPROPERTY(EditAnywhere)
	float camHideThreshold = 200.0f;

public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOverlappingWeapon(AWeapon* weapon);

	bool IsEquipped();

	bool IsAiming();

	AWeapon* GetEquippedWeapon();

	void PlayFireMontage(bool isAiming);
};

