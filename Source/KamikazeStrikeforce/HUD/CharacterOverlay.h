// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float hp, float maxHP);

private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* healthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* healthText;
	
};
