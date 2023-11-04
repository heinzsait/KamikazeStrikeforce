// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void AHitScanWeapon::Fire(const FVector hitLocation)
{
	Super::Fire(hitLocation);
		
	
	auto spawnSocket = GetWeaponMesh()->GetSocketByName(spawnSocketName);
	if (spawnSocket)
	{
		FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());

		FVector start = socketTransform.GetLocation();
		FVector end = useScatter ? TraceEndScatter(start, hitLocation) : (start + (hitLocation - start) * 1.25f);

		FHitResult result;
		UWorld* world = GetWorld();
		if (world)
		{
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(GetOwner());
			world->LineTraceSingleByChannel(result, start, end, ECC_Visibility, queryParams);
			FVector beamEnd = end;
			if (result.bBlockingHit)
			{
				if (HasAuthority())
				{
					AMainCharacter* _character = Cast<AMainCharacter>(result.GetActor());
					APawn* instigator = Cast<APawn>(GetOwner());
					if (_character && instigator && instigator->GetController())
					{
						UGameplayStatics::ApplyDamage(_character, damage, instigator->GetController(), this, UDamageType::StaticClass());
					}
				}

				if (impactFX)
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, impactFX, result.ImpactPoint, result.ImpactNormal.Rotation());
				}

				if (impactSFX)
				{
					UGameplayStatics::PlaySoundAtLocation(this, impactSFX, result.ImpactPoint);
				}
				beamEnd = result.ImpactPoint;
			}

			if (beamFX)
			{
				beamFXComp = UGameplayStatics::SpawnEmitterAtLocation(world, beamFX, socketTransform);
				if (beamFXComp)
				{
					beamFXComp->SetVectorParameter(FName("Target"), beamEnd);
				}
			}
		}
	}	
}

FVector AHitScanWeapon::TraceEndScatter(const FVector& start, const FVector& hitTarget)
{
	FVector ToTargetNormalized = (hitTarget - start).GetSafeNormal();
	FVector SphereCenter = start + ToTargetNormalized * distToSphere;
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, sphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	FVector ToEndLoc = EndLoc - start;

	return FVector(start + ToEndLoc * 100000 / ToEndLoc.Size());
}
