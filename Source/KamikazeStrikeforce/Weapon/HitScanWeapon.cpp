// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/PlayerController/MainPlayerController.h"
#include "KamikazeStrikeforce/Components/LagCompensationComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

void AHitScanWeapon::Fire(const FVector hitLocation)
{
	Super::Fire(hitLocation);		
	
	auto spawnSocket = GetWeaponMesh()->GetSocketByName(spawnSocketName);
	if (spawnSocket)
	{
		FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());

		FVector start = socketTransform.GetLocation();
		FVector end = (start + (hitLocation - start) * 1.25f);

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
				AMainCharacter* hitCharacter = Cast<AMainCharacter>(result.GetActor());
				APawn* instigator = Cast<APawn>(GetOwner());
				if (hitCharacter && instigator && instigator->GetController())
				{
					if(HasAuthority() && !useServerSideRewind)
						UGameplayStatics::ApplyDamage(hitCharacter, damage, instigator->GetController(), this, UDamageType::StaticClass());
					
					if (!HasAuthority() && useServerSideRewind)
					{
						if (!character) character = Cast<AMainCharacter>(GetOwner());
						if (character && character->Controller)
						{
							if (!playerController) playerController = Cast<AMainPlayerController>(character->Controller);
							if (playerController && character->GetLagCompensation())
							{
								character->GetLagCompensation()->ServerScoreRequest(hitCharacter, start, result.ImpactPoint, (playerController->GetServerTime() - playerController->singleTripTime), this);
							}
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
}

