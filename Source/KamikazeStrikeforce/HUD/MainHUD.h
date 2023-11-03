// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

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
class KAMIKAZESTRIKEFORCE_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public :

	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage& package) { HUDPackage = package; }

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> characterOverlayClass;

	class UCharacterOverlay* GetOverlay() const;

	void AddOverlay();


	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> gameInfoOverlayClass;

	class UGameInfoOverlay* GetGameInfoOverlay() const;

	void AddGameInfoOverlay();

protected:

	virtual void BeginPlay() override;

private:

	FHUDPackage HUDPackage;
	void DrawCrossHair(UTexture2D* tex, FVector2D center, FVector2D spread, FLinearColor color);

	float crossHairSpreadMax = 16.0f;

	UCharacterOverlay* characterOverlay = nullptr;
	UGameInfoOverlay* gameInfoOverlay = nullptr;

};
