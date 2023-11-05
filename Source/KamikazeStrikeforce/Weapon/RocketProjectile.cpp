// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

ARocketProjectile::ARocketProjectile()
{
	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Rocket Mesh"));
	projectileMesh->SetupAttachment(RootComponent);
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	rocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(FName("ProjectileMovementComponent"));
	rocketMovementComponent->bRotationFollowsVelocity = true;
	rocketMovementComponent->SetIsReplicated(true);
}


void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();

	SpawnTrailSystem();

	if(projectileLoopSFX && projectileLoopAttenuation)
		projectileLoopComp = UGameplayStatics::SpawnSoundAttached(projectileLoopSFX, RootComponent, FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.0f, 1.0f, 0.0f, projectileLoopAttenuation, (USoundConcurrency*)nullptr, false);

	if (!HasAuthority() && collisionBox)
		collisionBox->OnComponentHit.AddDynamic(this, &ARocketProjectile::OnHit);
	
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (OtherActor == GetOwner()) return;

	APawn* firingPawn = GetInstigator();
	if (firingPawn && HasAuthority())
	{
		AController* firingController = firingPawn->GetController();
		if (firingController)
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, damage, 10.f, GetActorLocation(), innerRadius, outerRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, firingController);
	}

	if (impactFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactFX, GetActorTransform());
	
	if (impactSFX)
		UGameplayStatics::PlaySoundAtLocation(this, impactSFX, GetActorLocation());
	
	if (projectileMesh)
		projectileMesh->SetVisibility(false);

	if (collisionBox)
		collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (trailFXComp && trailFXComp->GetSystemInstance())
		trailFXComp->GetSystemInstance()->Deactivate();

	if (projectileLoopComp && projectileLoopComp->IsPlaying())
		projectileLoopComp->Stop();

	StartDestroyTimer();

}

void ARocketProjectile::Destroyed()
{

}