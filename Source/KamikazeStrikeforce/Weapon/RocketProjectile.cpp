// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Kismet/GameplayStatics.h"

ARocketProjectile::ARocketProjectile()
{
	rocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("RocketMesh"));
	rocketMesh->SetupAttachment(RootComponent);
	rocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* firingPawn = GetInstigator();
	if (firingPawn)
	{
		AController* firingController = firingPawn->GetController();
		if (firingController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, damage, 10.f, GetActorLocation(), innerRadius, outerRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, firingController);
		}
	}

	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
