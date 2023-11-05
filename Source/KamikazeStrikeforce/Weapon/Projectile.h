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
	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* projectileMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* collisionBox;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UPROPERTY(EditAnywhere)
	float damage = 10.0f;

	UPROPERTY(EditAnywhere)
	float innerRadius = 200.0f;

	UPROPERTY(EditAnywhere)
	float outerRadius = 500.0f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* impactFX;

	UPROPERTY(EditAnywhere)
	class USoundBase* impactSFX;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* projectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* trailFX;

	UPROPERTY()
	class UNiagaraComponent* trailFXComp;

	void SpawnTrailSystem();

	void StartDestroyTimer();
	void DestroyTimerFinished();
private:	

	UPROPERTY(EditAnywhere)
	class UParticleSystem* tracerFX;

	UPROPERTY()
	class UParticleSystemComponent* tracerComp;

	FTimerHandle destroyTimer;

	UPROPERTY(EditAnywhere)
	float destroyTime = 3.0f;
};
