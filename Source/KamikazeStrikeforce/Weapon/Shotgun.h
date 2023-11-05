// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
		virtual void Fire(const FVector hitLocation) override;
		virtual void FireShotgun(const TArray<FVector_NetQuantize> hitLocations);

		void ShotgunTraceEndScatter(const FVector& hitTarget, TArray<FVector_NetQuantize>& hitTargets);

private:

	UPROPERTY(EditAnywhere)
	int noOfBullets = 10;

	UPROPERTY(EditAnywhere)
	USoundBase* fireSFX;
	
};
