// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInfoOverlay.h"
#include "Components/TextBlock.h"

void UGameInfoOverlay::SetWarmupTimer(FString time)
{
	if (warmupTime)
	{
		warmupTime->SetText(FText::FromString(time));
	}
}