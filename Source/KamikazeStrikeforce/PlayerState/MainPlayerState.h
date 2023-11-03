// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddScore(float score);
	virtual void OnRep_Score() override;
	void SetScoreHUD();
	void AddDeaths(int death);

private:

	class AMainPlayerController* playerController = nullptr;

	class AMainCharacter* character = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Deaths)
	int deaths = 0;

	UFUNCTION()
	void OnRep_Deaths();
	void SetDeathsHUD();
};
