// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class KAMIKAZESTRIKEFORCE_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float hp, float maxHP);
	void SetScore(float score);
	void SetDeaths(int deaths);
	void SetAmmo(int ammo);
	void SetCarriedAmmo(int carriedAmmo);
	void SetMatchTimer(FString time);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* highPingImg;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* highPingAnim;

private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* healthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* healthText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* scoreText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* deathsText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* ammoText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* carriedAmmoText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* matchTimerText;
	
};
