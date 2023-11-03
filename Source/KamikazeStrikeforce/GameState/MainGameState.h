// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

class AMainPlayerState;

UCLASS()
class KAMIKAZESTRIKEFORCE_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TArray<AMainPlayerState*> topPlayers;

	void UpdateTopScore(AMainPlayerState* scoringPlayer);

private:

	float topScore = 0.0f;
	
};
