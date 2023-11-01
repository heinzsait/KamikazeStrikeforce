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
	if (!IsLocalController()) return;
	if(!HUD) HUD = Cast<ABaseHUD>(GetHUD());
	
	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetHealth(hp, maxHP);
	}
}

void ABasePlayerController::SetHUDScore(float score)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetScore(score);
	}
}

void ABasePlayerController::SetHUDDeaths(int deaths)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetDeaths(deaths);
	}
}

void ABasePlayerController::SetHUDAmmo(int ammo)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetAmmo(ammo);
	}
}

void ABasePlayerController::SetHUDCarriedAmmo(int carriedAmmo)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetCarriedAmmo(carriedAmmo);
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