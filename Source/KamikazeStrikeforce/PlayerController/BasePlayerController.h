// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	void SetHUDHealth(float hp, float maxHP);
	void SetHUDScore(float score);
	void SetHUDDeaths(int deaths);
	void SetHUDAmmo(int ammo);
	void SetHUDCarriedAmmo(int carriedAmmo);

	virtual void OnPossess(APawn* InPawn) override;

protected:

private:

	class ABaseHUD* HUD = nullptr;

	class ABaseCharacter* playerCharacter = nullptr;
};
