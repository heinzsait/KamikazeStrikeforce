// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "KamikazeStrikeforce/HUD/BaseHUD.h"
#include "KamikazeStrikeforce/HUD/CharacterOverlay.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ABaseHUD>(GetHUD());
}

void ABasePlayerController::SetHUDHealth(float hp, float maxHP)
{
	if(!HUD) Cast<ABaseHUD>(GetHUD());
	
	if (HUD)
	{
		HUD->characterOverlay->SetHealth(hp, maxHP);
	}

}

void ABasePlayerController::SetHUDScore(float score)
{
	if (!HUD) Cast<ABaseHUD>(GetHUD());

	if (HUD)
	{
		HUD->characterOverlay->SetScore(score);
	}
}

void ABasePlayerController::SetHUDDeaths(int deaths)
{
	if (!HUD) Cast<ABaseHUD>(GetHUD());

	if (HUD)
	{
		HUD->characterOverlay->SetDeaths(deaths);
	}
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	playerCharacter = Cast<ABaseCharacter>(InPawn);
	if (playerCharacter)
	{
		SetHUDHealth(playerCharacter->GetHealth(), playerCharacter->GetMaxHealth());
	}
}