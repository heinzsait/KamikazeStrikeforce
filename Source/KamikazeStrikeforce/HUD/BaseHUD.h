// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:

	class UTexture2D* centerCH;
	UTexture2D* topCH;
	UTexture2D* bottomCH;
	UTexture2D* leftCH;
	UTexture2D* rightCH;
	float spreadSpeed;
	FLinearColor crosshairColor;
};

UCLASS()
class KAMIKAZESTRIKEFORCE_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public :

	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage& package) { HUDPackage = package; }

private:

	FHUDPackage HUDPackage;
	void DrawCrossHair(UTexture2D* tex, FVector2D center, FVector2D spread, FLinearColor color);

	float crossHairSpreadMax = 16.0f;

};
