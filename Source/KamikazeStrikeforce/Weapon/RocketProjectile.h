// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API ARocketProjectile : public AProjectile
{
	GENERATED_BODY()
public:
	ARocketProjectile();
	virtual void Destroyed() override;

protected:

	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	UPROPERTY(EditAnywhere)
	USoundBase* projectileLoopSFX;

	UPROPERTY()
	UAudioComponent* projectileLoopComp;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* projectileLoopAttenuation;

private:

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* rocketMovementComponent;
};
