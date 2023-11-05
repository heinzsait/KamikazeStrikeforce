// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
			FVector end = TraceEndScatter(hitLocation);
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

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize> hitLocations)
{
	AWeapon::Fire(FVector());

	auto spawnSocket = GetWeaponMesh()->GetSocketByName(spawnSocketName);
	if (spawnSocket)
	{
		FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());

		FVector start = socketTransform.GetLocation();

		TMap<AMainCharacter*, int> hitMap;
		UWorld* world = GetWorld();
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(GetOwner());

		for (int i = 0; i < hitLocations.Num(); i++)
		{
			FVector end = hitLocations[i];
			FHitResult result;
			if (world)
			{
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


void AShotgun::ShotgunTraceEndScatter(const FVector& hitTarget, TArray<FVector_NetQuantize>& hitTargets)
{
	auto spawnSocket = GetWeaponMesh()->GetSocketByName(spawnSocketName);
	if (!spawnSocket) return;
	const FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());
	const FVector start = socketTransform.GetLocation();
	const FVector ToTargetNormalized = (hitTarget - start).GetSafeNormal();
	const FVector SphereCenter = start + ToTargetNormalized * distToSphere;

	hitTargets.Empty();
	for (int i = 0; i < noOfBullets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, sphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - start;

		hitTargets.Add(FVector(start + ToEndLoc * 100000 / ToEndLoc.Size()));
	}
}
