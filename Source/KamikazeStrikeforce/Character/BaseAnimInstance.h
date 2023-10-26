// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
	ACharacter* character;
	UCharacterMovementComponent* characterMovement;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector velocity;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float groundSpeed;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool shouldMove;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isFalling;
};
