// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

void AShotgun::Fire(const FVector hitLocation)
{
	AWeapon::Fire(hitLocation);

	auto spawnSocket = GetWeaponMesh()->GetSocketByName(spawnSocketName);
	if (spawnSocket)
	{
		FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());

		FVector start = socketTransform.GetLocation();

		TMap<AMainCharacter*, int> hitMap;

		for (int i = 0; i < noOfBullets; i++)
		{
			FVector end = TraceEndScatter(start, hitLocation);
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
						if (_character)
						{
							if (hitMap.Contains(_character))
							{
								hitMap[_character]++;
							}
							else
							{
								hitMap.Emplace(_character, 1);
							}
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

		for (auto hit : hitMap)
		{
			if (HasAuthority())
			{
				AMainCharacter* _character = Cast<AMainCharacter>(hit.Key);
				APawn* instigator = Cast<APawn>(GetOwner());
				if (_character && instigator && instigator->GetController())
				{
					UGameplayStatics::ApplyDamage(_character, damage * hit.Value, instigator->GetController(), this, UDamageType::StaticClass());
				}
			}
		}

		if (fireSFX)
		{
			UGameplayStatics::PlaySoundAtLocation(this, fireSFX, GetActorLocation());
		}
	}
}
