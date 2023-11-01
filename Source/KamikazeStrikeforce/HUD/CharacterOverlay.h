// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class KAMIKAZESTRIKEFORCE_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float hp, float maxHP);
	void SetScore(float score);
	void SetDeaths(int deaths);

private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* healthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* healthText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* scoreText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* deathsText;
	
};
