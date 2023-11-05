// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:

	virtual void Fire(const FVector hitLocation) override;

protected:

	UPROPERTY(EditAnywhere)
	float damage = 20.0f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* impactFX;

	UPROPERTY(EditAnywhere)
	class USoundBase* impactSFX;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* beamFX;

	UPROPERTY()
	class UParticleSystemComponent* beamFXComp;

};
