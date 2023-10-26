// Copyright Epic Games, Inc. All Rights Reserved.

#include "KamikazeStrikeforceGameMode.h"
#include "Character/BaseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKamikazeStrikeforceGameMode::AKamikazeStrikeforceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_BaseCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
