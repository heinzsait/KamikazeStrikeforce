// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector hitLocation)
{
	Super::Fire(hitLocation);

	if (HasAuthority())
	{
		APawn* instigator = Cast<APawn>(GetOwner());
		auto spawnSocket = GetWeaponMesh()->GetSocketByName(spawnSocketName);
		if (spawnSocket && instigator)
		{
			FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());
			FRotator targetRot = (hitLocation - socketTransform.GetLocation()).Rotation();
			if (projectileClass)
			{
				if (GetWorld())
				{
					FActorSpawnParameters spawnParams;
					spawnParams.Owner = GetOwner();
					spawnParams.Instigator = instigator;
					GetWorld()->SpawnActor<AProjectile>(projectileClass, socketTransform.GetLocation(), targetRot, spawnParams);
				}
			}
		}
	}
}
