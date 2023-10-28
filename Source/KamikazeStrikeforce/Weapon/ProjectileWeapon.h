// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector hitLocation) override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> projectileClass;

	UPROPERTY(EditAnywhere)
	FName spawnSocketName = FName("MuzzleFlash");
};
