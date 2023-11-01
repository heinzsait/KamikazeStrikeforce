// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::SetHealth(float hp, float maxHP)
{
	if (healthBar && healthText)
	{
		healthBar->SetPercent(hp / maxHP);

		healthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(hp), FMath::CeilToInt(maxHP))));
	}
}

void UCharacterOverlay::SetScore(float score)
{
	if (scoreText)
	{
		scoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), FMath::CeilToInt(score))));
	}
}

void UCharacterOverlay::SetDeaths(int deaths)
{
	if (deathsText)
	{
		deathsText->SetText(FText::FromString(FString::Printf(TEXT("Deaths: %d"), deaths)));
	}
}

void UCharacterOverlay::SetAmmo(int ammo)
{
	if (ammoText)
	{
		ammoText->SetText(FText::FromString(FString::Printf(TEXT("Ammo: %d"), ammo)));
	}
}

void UCharacterOverlay::SetCarriedAmmo(int carriedAmmo)
{
	if (carriedAmmoText)
	{
		carriedAmmoText->SetText(FText::FromString(FString::Printf(TEXT("/ %d"), carriedAmmo)));
	}
}
