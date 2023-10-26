// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	character = Cast<ACharacter>(GetOwningActor());
	if (character)
	{
		characterMovement = character->GetCharacterMovement();
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (character && characterMovement)
	{
		velocity = characterMovement->Velocity;
		groundSpeed = UKismetMathLibrary::VSizeXY(velocity);

		shouldMove = ((groundSpeed > 3.0f) && (!characterMovement->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.0f)));

		isFalling = characterMovement->IsFalling();
	}
}
