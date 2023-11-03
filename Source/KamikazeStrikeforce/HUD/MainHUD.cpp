// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "GameInfoOverlay.h"


void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
}


void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D viewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewPortSize);
		const FVector2D viewPortCenter(viewPortSize.X / 2, viewPortSize.Y / 2);

		float spreadScale = HUDPackage.spreadSpeed * crossHairSpreadMax;
		FLinearColor color = HUDPackage.crosshairColor;

		if (HUDPackage.centerCH)
			DrawCrossHair(HUDPackage.centerCH, viewPortCenter, FVector2D(0.f, 0.f), color);

		if (HUDPackage.topCH)
			DrawCrossHair(HUDPackage.topCH, viewPortCenter, FVector2D(0.f, -spreadScale), color);

		if (HUDPackage.bottomCH)
			DrawCrossHair(HUDPackage.bottomCH, viewPortCenter, FVector2D(0.f, spreadScale), color);

		if (HUDPackage.leftCH)
			DrawCrossHair(HUDPackage.leftCH, viewPortCenter, FVector2D(-spreadScale, 0.f), color);

		if (HUDPackage.rightCH)
			DrawCrossHair(HUDPackage.rightCH, viewPortCenter, FVector2D(spreadScale, 0.f), color);
	}
}


void AMainHUD::DrawCrossHair(UTexture2D* tex, FVector2D center, FVector2D spread, FLinearColor color)
{
	const float texWidth = tex->GetSizeX();
	const float texHeight = tex->GetSizeY();
	const FVector2D texDrawPoint(center.X - (texWidth / 2.f) + spread.X, center.Y - (texHeight / 2.f) + spread.Y);

	DrawTexture(tex, texDrawPoint.X, texDrawPoint.Y, texWidth, texHeight, 0.0f, 0.0f, 1.0f, 1.0f, color);
}

void AMainHUD::AddOverlay()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && characterOverlayClass)
	{
		characterOverlay = CreateWidget<UCharacterOverlay>(playerController, characterOverlayClass);
		characterOverlay->AddToViewport();
	}
}

UCharacterOverlay* AMainHUD::GetOverlay() const
{
	return characterOverlay;
}

void AMainHUD::AddGameInfoOverlay()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && gameInfoOverlayClass)
	{
		gameInfoOverlay = CreateWidget<UGameInfoOverlay>(playerController, gameInfoOverlayClass);
		gameInfoOverlay->AddToViewport();
	}
}

UGameInfoOverlay* AMainHUD::GetGameInfoOverlay() const
{
	return gameInfoOverlay;
}
