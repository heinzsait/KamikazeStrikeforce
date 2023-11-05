// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerState.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/PlayerController/MainPlayerController.h"
#include "Net/UnrealNetwork.h"



void AMainPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPlayerState, deaths);
}

void AMainPlayerState::AddScore(float score)
{
	SetScore(GetScore() + score);

	SetScoreHUD();
}

void AMainPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	SetScoreHUD();
}

void AMainPlayerState::SetScoreHUD()
{
	if (!character) character = Cast<AMainCharacter>(GetPawn());
	if (character)
	{
		if (!playerController && character->GetController()) playerController = Cast<AMainPlayerController>(character->GetController());
		if (playerController)
		{
			playerController->SetHUDScore(GetScore());
		}
	}
}

void AMainPlayerState::AddDeaths(int death)
{
	deaths += death;

	SetDeathsHUD();
}

void AMainPlayerState::OnRep_Deaths()
{
	SetDeathsHUD();
}

void AMainPlayerState::SetDeathsHUD()
{
	if (!character) character = Cast<AMainCharacter>(GetPawn());
	if (character)
	{
		if (!playerController) playerController = Cast<AMainPlayerController>(character->GetController());
		if (playerController)
		{
			playerController->SetHUDDeaths(deaths);
		}
	}
}
