// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "KamikazeStrikeforceGameMode.generated.h"

UCLASS(minimalapi)
class AKamikazeStrikeforceGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AKamikazeStrikeforceGameMode();

	virtual void PlayerEliminated(class ABaseCharacter* eliminatedCharacter, class ABasePlayerController* victimController, class ABasePlayerController* attackerController);

	virtual void RequestRespawn(ABaseCharacter* eliminatedCharacter, ABasePlayerController* eliminatedController);
};



