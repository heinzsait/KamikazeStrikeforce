// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameInfoOverlay.generated.h"

class UTextBlock;

UCLASS()
class KAMIKAZESTRIKEFORCE_API UGameInfoOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetWarmupTimer(FString time);

	void SetInfoText(FString text);
private:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* warmupTime;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* infoText;
};
