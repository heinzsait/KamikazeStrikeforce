// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	collisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Collision Box"));
	SetRootComponent(collisionBox);

	collisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	collisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	collisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	collisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	collisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (tracerFX)
	{
		tracerComp = UGameplayStatics::SpawnEmitterAttached(tracerFX, collisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	if (HasAuthority() && collisionBox)
	{
		collisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::SpawnTrailSystem()
{
	if (trailFX)
		trailFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(trailFX, RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(destroyTimer, this, &AProjectile::DestroyTimerFinished, destroyTime);
}

void AProjectile::DestroyTimerFinished()
{
	Destroy();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (impactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactFX, GetActorTransform());
	}
	if (impactSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, impactSFX, GetActorLocation());
	}

}

