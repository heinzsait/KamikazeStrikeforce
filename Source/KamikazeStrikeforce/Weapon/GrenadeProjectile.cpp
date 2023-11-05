// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Grenade Mesh"));
	projectileMesh->SetupAttachment(RootComponent);
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovementComponent"));
	projectileMovementComponent->bRotationFollowsVelocity = true;
	projectileMovementComponent->SetIsReplicated(true);
	projectileMovementComponent->bShouldBounce = true;
}

void AGrenadeProjectile::BeginPlay()
{
	AActor::BeginPlay();

	StartDestroyTimer();
	SpawnTrailSystem();

	projectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AGrenadeProjectile::OnBounce);
}

void AGrenadeProjectile::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (bounceSFX)
		UGameplayStatics::PlaySoundAtLocation(this, bounceSFX, GetActorLocation());
}

void AGrenadeProjectile::Destroyed()
{
	APawn* firingPawn = GetInstigator();
	if (firingPawn && HasAuthority())
	{
		AController* firingController = firingPawn->GetController();
		if (firingController)
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, damage, 10.f, GetActorLocation(), innerRadius, outerRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, firingController);
	}

	Super::Destroyed();
}