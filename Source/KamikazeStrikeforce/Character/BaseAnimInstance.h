// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "BaseAnimInstance.generated.h"

class UCharacterMovementComponent;

UCLASS()
class KAMIKAZESTRIKEFORCE_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	
private:
	class ABaseCharacter* character;
	UCharacterMovementComponent* characterMovement;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector velocity;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float groundSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool shouldMove;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isAccelerating;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isWeaponEquiped;

	class AWeapon* equippedWeapon;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isCrouched;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isAiming;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float yawOffset;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float lean;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator deltaRotation;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform leftHandTransform;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ETurnInPlace turnInPlace;
};
