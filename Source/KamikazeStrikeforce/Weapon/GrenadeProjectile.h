// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "GrenadeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API AGrenadeProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	AGrenadeProjectile();
	virtual void Destroyed() override;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce( const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:

	UPROPERTY(EditAnywhere)
	class USoundBase* bounceSFX;
};
