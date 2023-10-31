// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "KamikazeStrikeforce/HUD/BaseHUD.h"
#include "KamikazeStrikeforce/HUD/CharacterOverlay.h"

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
