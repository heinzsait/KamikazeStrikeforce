// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer && NewPlayer->HasAuthority() && NewPlayer->IsLocalController())
		serverPlayerController = NewPlayer;

	int32 numOfPlayers = GameState.Get()->PlayerArray.Num();
	if (numOfPlayers == 2)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			//world->ServerTravel(FString("/Game/Maps/Asian_Village?listen"));
		}
	}
}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	if (serverPlayerController)
	{
		if (serverPlayerController->IsInputKeyDown(EKeys::M))
		{
			UWorld* world = GetWorld();
			if (world)
			{
				world->ServerTravel(FString("/Game/Maps/Asian_Village?listen"));
			}
		}
	}
}
