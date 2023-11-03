// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "KamikazeStrikeforceGameMode.generated.h"

namespace MatchState
{
	extern KAMIKAZESTRIKEFORCE_API const FName Cooldown;
}

UCLASS()
class KAMIKAZESTRIKEFORCE_API AKamikazeStrikeforceGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AKamikazeStrikeforceGameMode();

	virtual void Tick(float DeltaTime) override;

	virtual void PlayerEliminated(class ABaseCharacter* eliminatedCharacter, class ABasePlayerController* victimController, class ABasePlayerController* attackerController);

	virtual void RequestRespawn(ABaseCharacter* eliminatedCharacter, ABasePlayerController* eliminatedController);

	UPROPERTY(EditDefaultsOnly)
	float warmupTime = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float matchTime = 120.0f;

	UPROPERTY(EditDefaultsOnly)
	float cooldownTime = 10.0f;

	float levelStartTime = 0.0f;

	FORCEINLINE float GetCountDownTime() const { return countDownTime; }

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:

	float countDownTime = 0.0f;
};



