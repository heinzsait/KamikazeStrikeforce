// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	character = Cast<ABaseCharacter>(GetOwningActor());
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

		isAccelerating = characterMovement->GetCurrentAcceleration().Size() > 0.f ? true : false;

		isWeaponEquiped = character->IsEquipped();

		isCrouched = character->bIsCrouched;

		isAiming = character->IsAiming();

		// Offset Yaw for Strafing
		FRotator AimRotation = character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(character->GetVelocity());
		FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		deltaRotation = FMath::RInterpTo(deltaRotation, deltaRot, DeltaSeconds, 6.0f);
		yawOffset = deltaRotation.Yaw;

		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = character->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaSeconds;
		const float Interp = FMath::FInterpTo(lean, Target, DeltaSeconds, 6.0f);
		lean = FMath::Clamp(Interp, -90.f, 90.f);
	}
}
