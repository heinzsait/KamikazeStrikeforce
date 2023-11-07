// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector location;

	UPROPERTY()
	FRotator rotation;

	UPROPERTY()
	FVector boxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float time;

	UPROPERTY()
	TMap<FName, FBoxInformation> hitBoxInfo;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KAMIKAZESTRIKEFORCE_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend class AMainCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(AMainCharacter* hitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& hitLocation, float hitTime, AWeapon* damageCauser);

	FServerSideRewindResult ServerSideRewind(AMainCharacter* hitCharacter, const FVector_NetQuantize& traceStart, const FVector_NetQuantize& hitLocation, float hitTime);

	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& package);
private:

	void SaveFramePackages();

	UPROPERTY()
	class AMainCharacter* character;

	UPROPERTY()
	class AMainPlayerController* playerController;

	TDoubleLinkedList<FFramePackage> frameHistory;

	UPROPERTY(EditAnywhere)
	float maxRecTime = 4.0f;

	FFramePackage InterpBetweenFrames(const FFramePackage& olderFrame, const FFramePackage& youngerFrame, float hitTime);
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package,AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);
	void CacheBoxPositions(AMainCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AMainCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
};

/*
Copy these lines to DefaultEngine.ini to simulate lag

[PacketSimulationSettings]
pktLag = 500

*/