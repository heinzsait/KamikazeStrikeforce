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

protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UPROPERTY(EditAnywhere)
	float innerRadius = 200.0f;

	UPROPERTY(EditAnywhere)
	float outerRadius = 500.0f;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* rocketMesh;
};
