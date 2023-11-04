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

	FVector TraceEndScatter(const FVector& start, const FVector& hitTarget);
	
	UPROPERTY(EditAnywhere)
	FName spawnSocketName = FName("MuzzleFlash");

	UPROPERTY(EditAnywhere)
	class UParticleSystem* impactFX;

	UPROPERTY(EditAnywhere)
	class USoundBase* impactSFX;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* beamFX;

	UPROPERTY()
	class UParticleSystemComponent* beamFXComp;

private:

	UPROPERTY(EditAnywhere, Category = Scatter)
	float distToSphere = 800.0f;

	UPROPERTY(EditAnywhere, Category = Scatter)
	float sphereRadius = 75.0f;

	UPROPERTY(EditAnywhere, Category = Scatter)
	bool useScatter = false;
};
