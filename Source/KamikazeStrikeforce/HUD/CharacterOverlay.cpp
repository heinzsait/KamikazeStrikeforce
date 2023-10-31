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
