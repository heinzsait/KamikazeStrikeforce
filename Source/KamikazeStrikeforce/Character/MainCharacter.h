// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "KamikazeStrikeforce/Interfaces/CrosshairHitInterface.h"
#include "Components/TimelineComponent.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCombatComponent;
class AMainPlayerController;
class AMainPlayerState;
class AMainHUD;
class UAnimMontage;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AMainCharacter : public ACharacter, public ICrosshairHitInterface
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

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;

	void CheckInputContext();

	UPROPERTY(Replicated)
	bool disableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScope(bool show);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void JumpPressed();
	void JumpReleased();

	void EquipPressed();
	void CrouchPressed();

	UFUNCTION(Server, Reliable)
	void ServerEquipPressed();


	void AimPressed();
	void AimReleased();

	void FirePressed();
	void FireReleased();

	void ReloadPressed();

	void AimOffset(float deltaTime);

	void CalculateAO_Pitch();

	void UpdateTurnInPlace(float deltaTime);

	void SimProxiesTurn();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void UpdatePlayerHUDHealth();

	void PlayEliminationMontage();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }

	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	FORCEINLINE ETurnInPlace GetTurnInPlace() const { return turnInPlace; }

	AMainPlayerController* GetPlayerController();

	AMainHUD* GetMainHUD();

	FORCEINLINE UCameraComponent* GetCamera() const { return FollowCamera; }

	FORCEINLINE bool ShouldRotateRootBone() const { return rotateRootBone; }

	FORCEINLINE float GetHealth() const { return health; }
	FORCEINLINE float GetMaxHealth() const { return maxHealth; }

	ECombatState GetCombatState() const;

	FVector GetHitLocation();

	virtual void OnRep_ReplicatedMovement() override;

	void Eliminate();
	bool isEliminated = false;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminate();

	UFUNCTION(BlueprintCallable)
	UCombatComponent* GetCombat() const;

private:

	AMainPlayerController* playerController = nullptr;

	AMainPlayerState* basePlayerState = nullptr;

	AMainHUD* mainHUD = nullptr;

	bool inputContextNotSet = false;

	void PollInitializePlayerState();
	bool playerStateSet = false;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = Stats)
	float health = 100.0f; 

	UPROPERTY(EditAnywhere, Category = Stats)
	float maxHealth = 100.0f;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* overlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* lastWeapon);

	float AO_Yaw;
	float AO_Pitch;
	float interpAO_Yaw;
	FRotator startAimRot;

	ETurnInPlace turnInPlace;
	bool rotateRootBone;
	FRotator proxyRot;
	FRotator proxyPrevRot;
	float proxyYaw;
	float lastRepMovementUpdateTime;

	UPROPERTY()
	class UCharacterAnimInstance* animInstance;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* fireMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* reloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* hitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* eliminationMontage;

	void HideCamIfCharClose();

	UPROPERTY(EditAnywhere)
	float camHideThreshold = 200.0f;

	FTimerHandle eliminationTimer;
	void EliminationFinished();

	UPROPERTY(EditDefaultsOnly)
	float eliminiationDelay = 3.0f;

	//Dissolve Effect...

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* dissolveTimeline;
	FOnTimelineFloat dissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* dissolveCurve;

	UPROPERTY(EditAnywhere)
	float startDissolveValue;

	UFUNCTION()
	void UpdateDissolveMaterial(float dissolveValue);
	void StartDissolve();

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere)
	TArray<UMaterialInstanceDynamic*> dynamicDissolveMaterialInstances;

	// Material instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> dissolveMaterialInstances;

public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOverlappingWeapon(AWeapon* weapon);

	bool IsEquipped();

	bool IsAiming();

	AWeapon* GetEquippedWeapon();

	void PlayFireMontage(bool isAiming);

	void PlayReloadMontage();

	void PlayHitReactMontage();

	/*UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitReact();*/
};

