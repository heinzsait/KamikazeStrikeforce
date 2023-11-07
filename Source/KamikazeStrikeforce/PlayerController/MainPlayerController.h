// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KAMIKAZESTRIKEFORCE_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;


	void CheckTimeSync(float DeltaTime);

	void SetHUDHealth(float hp, float maxHP);
	void SetHUDScore(float score);
	void SetHUDDeaths(int deaths);
	void SetHUDAmmo(int ammo);
	void SetHUDCarriedAmmo(int carriedAmmo);
	void SetHUDMatchTimer(float timer);
	void SetHUDMatchInfoTimer(float timer);
	void SetHUDTime();

	virtual void ReceivedPlayer() override;

	virtual float GetServerTime();
	float singleTripTime = 0.0f;

	float matchTime = 0.0f;
	float warmupTime = 0.0f;
	float cooldownTime = 0.0f;
	float levelStartTime = 0.0f;

	void OnMatchStateSet(FName state);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	void HandleMatchStarted();
	void HandleCooldown();

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Avatar")
	EAvatar playerAvatar = EAvatar::None;

protected:

	virtual void OnPossess(APawn* aPawn) override;

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float timeOfClientReq);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float timeOfClientReq, float timeServerReceivedClientReq);

	float serverClientDelta = 0.0f;

	UPROPERTY(EditAnywhere)
	float timeSyncInterval = 5.0f;
	float timeSyncRunningTime = 0.0f;

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

private:

	class AMainHUD* HUD = nullptr;

	class AMainCharacter* playerCharacter = nullptr;

	class AKamikazeStrikeforceGameMode* gameMode = nullptr;

	int countDown = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName matchState;

	UFUNCTION()
	void OnRep_MatchState();


	UFUNCTION(Client, Reliable)
	void ClientJoin(FName _state, float _warmupTime, float _matchTime, float _startTtime, float _cooldownTime);

	UPROPERTY(EditAnywhere)
	float highPingRunningTime = 0;

	UPROPERTY(EditAnywhere)
	float highPingDuration = 5.0f;

	UPROPERTY(EditAnywhere)
	float checkPingInterval = 20.0f;

	UPROPERTY(EditAnywhere)
	float highPingThreshold = 50.0f;

	float highPingAnimRunningTime = 0;

	void LoadGameFromSave();

	UFUNCTION(Server, Reliable)
	void ServerInitAvatar(EAvatar _avatar);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInitAvatar(EAvatar _avatar);

	bool infoNotSet = false;
};
