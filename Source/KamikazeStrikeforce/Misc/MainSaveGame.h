// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "MainSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API UMainSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	EAvatar playerAvatar;
};
