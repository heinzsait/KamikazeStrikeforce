// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& package)
{
	if (!character) character = Cast<AMainCharacter>(GetOwner());
	if (character)
	{
		package.time = GetWorld()->GetTimeSeconds();
		for (auto& hitBox : character->hitCollisionBoxes)
		{
			FBoxInformation info;
			info.location = hitBox.Value->GetComponentLocation();
			info.rotation = hitBox.Value->GetComponentRotation();
			info.boxExtent = hitBox.Value->GetScaledBoxExtent();

			package.hitBoxInfo.Add(hitBox.Key, info);
		}
	}
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package, AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if (HitCharacter == nullptr) return FServerSideRewindResult();

	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	// Enable collision for the head first
	UBoxComponent* HeadBox = HitCharacter->hitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	FHitResult ConfirmHitResult;
	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	UWorld* World = GetWorld();
	if (World)
	{
		World->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECollisionChannel::ECC_Visibility
		);
		if (ConfirmHitResult.bBlockingHit) // we hit the head, return early
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{ true, true };
		}
		else // didn't hit head, check the rest of the boxes
		{
			for (auto& HitBoxPair : HitCharacter->hitCollisionBoxes)
			{
				if (HitBoxPair.Value != nullptr)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
				}
			}
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECollisionChannel::ECC_Visibility
			);
			if (ConfirmHitResult.bBlockingHit)
			{
				ResetHitBoxes(HitCharacter, CurrentFrame);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
				return FServerSideRewindResult{ true, false };
			}
		}
	}

	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{ false, false };
}

void ULagCompensationComponent::CacheBoxPositions(AMainCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->hitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInfo;
			BoxInfo.location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.boxExtent = HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.hitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
		}
	}
}

void ULagCompensationComponent::MoveBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->hitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.hitBoxInfo[HitBoxPair.Key].location);
			HitBoxPair.Value->SetWorldRotation(Package.hitBoxInfo[HitBoxPair.Key].rotation);
			HitBoxPair.Value->SetBoxExtent(Package.hitBoxInfo[HitBoxPair.Key].boxExtent);
		}
	}
}

void ULagCompensationComponent::ResetHitBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->hitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.hitBoxInfo[HitBoxPair.Key].location);
			HitBoxPair.Value->SetWorldRotation(Package.hitBoxInfo[HitBoxPair.Key].rotation);
			HitBoxPair.Value->SetBoxExtent(Package.hitBoxInfo[HitBoxPair.Key].boxExtent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ULagCompensationComponent::EnableCharacterMeshCollision(AMainCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled)
{
	if (HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
}


// Called every frame
void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFramePackages();
}

void ULagCompensationComponent::SaveFramePackages()
{
	if (character && character->HasAuthority())
	{
		if (frameHistory.Num() <= 1)
		{
			FFramePackage ThisFrame;
			SaveFramePackage(ThisFrame);
			frameHistory.AddHead(ThisFrame);
		}
		else
		{
			float historyLength = frameHistory.GetHead()->GetValue().time - frameHistory.GetTail()->GetValue().time;
			while (historyLength > maxRecTime)
			{
				frameHistory.RemoveNode(frameHistory.GetTail());
				historyLength = frameHistory.GetHead()->GetValue().time - frameHistory.GetTail()->GetValue().time;
			}
			FFramePackage thisFrame;
			SaveFramePackage(thisFrame);
			frameHistory.AddHead(thisFrame);

			//ShowFramePackage(thisFrame, FColor::Red);
		}
	}
}

void ULagCompensationComponent::ServerScoreRequest_Implementation(AMainCharacter* hitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& hitLocation, float hitTime, AWeapon* damageCauser)
{
	FServerSideRewindResult Confirm = ServerSideRewind(hitCharacter, traceStart, hitLocation, hitTime);

	if (character && hitCharacter && damageCauser && Confirm.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(
			hitCharacter,
			damageCauser->GetDamage(),
			character->Controller,
			damageCauser,
			UDamageType::StaticClass()
		);
	}
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(AMainCharacter* hitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& hitLocation, float hitTime)
{
	bool bReturn =
		hitCharacter == nullptr ||
		hitCharacter->GetLagCompensation() == nullptr ||
		hitCharacter->GetLagCompensation()->frameHistory.GetHead() == nullptr ||
		hitCharacter->GetLagCompensation()->frameHistory.GetTail() == nullptr;

	if (bReturn) return FServerSideRewindResult();

	FFramePackage frameToCheck;
	bool shouldInterpolate = true;

	const TDoubleLinkedList<FFramePackage>& history = hitCharacter->GetLagCompensation()->frameHistory;
	const float oldestHistoryTime = history.GetTail()->GetValue().time;
	const float newestHistoryTime = history.GetHead()->GetValue().time;
	if (oldestHistoryTime > hitTime)
	{
		return FServerSideRewindResult();
	}
	if (oldestHistoryTime == hitTime)
	{
		frameToCheck = history.GetTail()->GetValue();
		shouldInterpolate = false;
	}
	if (newestHistoryTime <= hitTime)
	{
		frameToCheck = history.GetHead()->GetValue();
		shouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* younger = history.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* older = younger;
	while (older->GetValue().time > hitTime)
	{
		if (older->GetNextNode() == nullptr) break;
		older = older->GetNextNode();
		if (older->GetValue().time > hitTime)
		{
			younger = older;
		}
	}
	if (older->GetValue().time == hitTime)
	{
		frameToCheck = older->GetValue();
		shouldInterpolate = false;
	}
	if (shouldInterpolate)
	{
		frameToCheck = InterpBetweenFrames(older->GetValue(), younger->GetValue(), hitTime);
	}

	return ConfirmHit(frameToCheck, hitCharacter, traceStart, hitLocation);
}

FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& olderFrame, const FFramePackage& youngerFrame, float hitTime)
{
	const float distance = youngerFrame.time - olderFrame.time;
	const float interpFraction = FMath::Clamp((hitTime - olderFrame.time) / distance, 0.f, 1.f);

	FFramePackage interpFramePackage;
	interpFramePackage.time = hitTime;

	for (auto& youngerPair : youngerFrame.hitBoxInfo)
	{
		const FName& boxInfoName = youngerPair.Key;

		const FBoxInformation& olderBox = olderFrame.hitBoxInfo[boxInfoName];
		const FBoxInformation& youngerBox = youngerFrame.hitBoxInfo[boxInfoName];

		FBoxInformation interpBoxInfo;

		interpBoxInfo.location = FMath::VInterpTo(olderBox.location, youngerBox.location, 1.f, interpFraction);
		interpBoxInfo.rotation = FMath::RInterpTo(olderBox.rotation, youngerBox.rotation, 1.f, interpFraction);
		interpBoxInfo.boxExtent = youngerBox.boxExtent;

		interpFramePackage.hitBoxInfo.Add(boxInfoName, interpBoxInfo);
	}

	return interpFramePackage;
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxInfo : Package.hitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.location,
			BoxInfo.Value.boxExtent,
			FQuat(BoxInfo.Value.rotation),
			Color,
			false,
			3.0f
		);
	}
}