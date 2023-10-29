// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class KAMIKAZESTRIKEFORCE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

private:	
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* collisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* projectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* tracerFX;

	class UParticleSystemComponent* tracerComp;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* impactFX;

	UPROPERTY(EditAnywhere)
	class USoundBase* impactSFX;
};