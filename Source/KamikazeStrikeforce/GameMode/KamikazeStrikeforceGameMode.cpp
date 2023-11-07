// Copyright Epic Games, Inc. All Rights Reserved.

#include "KamikazeStrikeforceGameMode.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/PlayerController/MainPlayerController.h"
#include "KamikazeStrikeforce/PlayerState/MainPlayerState.h"
#include "KamikazeStrikeforce/GameState/MainGameState.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AKamikazeStrikeforceGameMode::AKamikazeStrikeforceGameMode()
{
	bDelayedStart = true;
}

void AKamikazeStrikeforceGameMode::BeginPlay()
{
	Super::BeginPlay();

	levelStartTime = GetWorld()->GetTimeSeconds();
}

void AKamikazeStrikeforceGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		AMainPlayerController* playerCont = Cast<AMainPlayerController>(*it);
		if (playerCont)
		{
			playerCont->OnMatchStateSet(MatchState);	
		}
	}
}

void AKamikazeStrikeforceGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		countDownTime = warmupTime - GetWorld()->GetTimeSeconds() + levelStartTime;
		if (countDownTime <= 0)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		countDownTime = matchTime + warmupTime - GetWorld()->GetTimeSeconds() + levelStartTime;
		if (countDownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		countDownTime = cooldownTime + matchTime + warmupTime - GetWorld()->GetTimeSeconds() + levelStartTime;
		if (countDownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void AKamikazeStrikeforceGameMode::PlayerEliminated(AMainCharacter* eliminatedCharacter, AMainPlayerController* victimController, AMainPlayerController* attackerController)
{
	if (victimController && attackerController && attackerController->PlayerState && victimController->PlayerState)
	{
		AMainPlayerState* attackerState = Cast<AMainPlayerState>(attackerController->PlayerState);
		AMainPlayerState* victimState = Cast<AMainPlayerState>(victimController->PlayerState);

		AMainGameState* gameState = GetGameState<AMainGameState>();

		if (attackerState && victimState && attackerState != victimState)
		{
			attackerState->AddScore(1.0f);

			if (gameState)
			{
				gameState->UpdateTopScore(attackerState);
			}
		}

		if(victimState)
			victimState->AddDeaths(1);

	}

	if(eliminatedCharacter)
		eliminatedCharacter->Eliminate();
}

void AKamikazeStrikeforceGameMode::RequestRespawn(AMainCharacter* eliminatedCharacter, AMainPlayerController* eliminatedController)
{
	if (eliminatedCharacter)
	{
		eliminatedCharacter->Reset();
		eliminatedCharacter->Destroy();
	}

	if(eliminatedController)
	{
		TArray<AActor*> playerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);

		RestartPlayerAtPlayerStart(eliminatedController, playerStarts[FMath::RandRange(0, playerStarts.Num() - 1)]);
	}
}
