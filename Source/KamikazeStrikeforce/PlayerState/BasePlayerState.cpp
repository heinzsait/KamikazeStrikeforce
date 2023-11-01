// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"
#include "KamikazeStrikeforce/PlayerController/BasePlayerController.h"
#include "Net/UnrealNetwork.h"



void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerState, deaths);
}

void ABasePlayerState::AddScore(float score)
{
	SetScore(GetScore() + score);

	SetScoreHUD();
}

void ABasePlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	SetScoreHUD();
}

void ABasePlayerState::SetScoreHUD()
{
	if (!character) character = Cast<ABaseCharacter>(GetPawn());
	if (character)
	{
		if (!playerController) playerController = Cast<ABasePlayerController>(character->GetController());
		if (playerController)
		{
			playerController->SetHUDScore(GetScore());
		}
	}
}

void ABasePlayerState::AddDeaths(int death)
{
	deaths += death;

	SetDeathsHUD();
}

void ABasePlayerState::OnRep_Deaths()
{
	SetDeathsHUD();
}

void ABasePlayerState::SetDeathsHUD()
{
	if (!character) character = Cast<ABaseCharacter>(GetPawn());
	if (character)
	{
		if (!playerController) playerController = Cast<ABasePlayerController>(character->GetController());
		if (playerController)
		{
			playerController->SetHUDDeaths(deaths);
		}
	}
}
