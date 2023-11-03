// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Net/UnrealNetwork.h"
#include "KamikazeStrikeforce/PlayerState/MainPlayerState.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainGameState, topPlayers);
}

void AMainGameState::UpdateTopScore(AMainPlayerState* scoringPlayer)
{
	if (topPlayers.Num() == 0)
	{
		topPlayers.Add(scoringPlayer);
		topScore = scoringPlayer->GetScore();
	}
	else if (scoringPlayer->GetScore() == topScore)
	{
		topPlayers.AddUnique(scoringPlayer);
	}
	else if (scoringPlayer->GetScore() > topScore)
	{
		topPlayers.Empty();
		topPlayers.AddUnique(scoringPlayer);
		topScore = scoringPlayer->GetScore();
	}
}
