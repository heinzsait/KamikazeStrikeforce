// Copyright Epic Games, Inc. All Rights Reserved.

#include "KamikazeStrikeforceGameMode.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"
#include "KamikazeStrikeforce/PlayerController/BasePlayerController.h"
#include "KamikazeStrikeforce/PlayerState/BasePlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

AKamikazeStrikeforceGameMode::AKamikazeStrikeforceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_BaseCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AKamikazeStrikeforceGameMode::PlayerEliminated(ABaseCharacter* eliminatedCharacter, ABasePlayerController* victimController, ABasePlayerController* attackerController)
{
	if (eliminatedCharacter && victimController && attackerController)
	{
		eliminatedCharacter->Eliminate();

		ABasePlayerState* attackerState = Cast<ABasePlayerState>(attackerController->PlayerState);
		ABasePlayerState* victimState = Cast<ABasePlayerState>(victimController->PlayerState);

		if (attackerState && attackerState != victimState)
		{
			attackerState->AddScore(1.0f);
			victimState->AddDeaths(1);
		}
	}
}

void AKamikazeStrikeforceGameMode::RequestRespawn(ABaseCharacter* eliminatedCharacter, ABasePlayerController* eliminatedController)
{
	if (eliminatedCharacter && eliminatedController)
	{
		eliminatedCharacter->Reset();
		eliminatedCharacter->Destroy();

		TArray<AActor*> playerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);

		RestartPlayerAtPlayerStart(eliminatedController, playerStarts[FMath::RandRange(0, playerStarts.Num() - 1)]);
	}
}
