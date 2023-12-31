// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBullet::AProjectileBullet()
{
	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovementComponent"));
	projectileMovementComponent->bRotationFollowsVelocity = true;
	projectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* ownerChar = Cast<ACharacter>(GetOwner());
	if (ownerChar)
	{
		AController* ownerController = ownerChar->GetController();
		if (ownerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, damage, ownerController, this, UDamageType::StaticClass());
		}
	}
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
