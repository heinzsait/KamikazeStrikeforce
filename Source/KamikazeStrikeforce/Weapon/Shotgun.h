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

private:

	UPROPERTY(EditAnywhere)
	int noOfBullets = 10;

	UPROPERTY(EditAnywhere)
	USoundBase* fireSFX;
	
};
