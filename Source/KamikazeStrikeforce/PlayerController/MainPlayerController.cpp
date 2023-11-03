// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "KamikazeStrikeforce/HUD/MainHUD.h"
#include "KamikazeStrikeforce/HUD/CharacterOverlay.h"
#include "KamikazeStrikeforce/HUD/GameInfoOverlay.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/Components/CombatComponent.h"
#include "KamikazeStrikeforce/GameMode/KamikazeStrikeforceGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AMainHUD>(GetHUD());	
	//if(HUD) HUD->AddGameInfoOverlay();
	ServerCheckMatchState();
}

void AMainPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPlayerController, matchState);
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);
}

void AMainPlayerController::CheckTimeSync(float DeltaTime)
{
	timeSyncRunningTime += DeltaTime;
	if (IsLocalController() && timeSyncRunningTime > timeSyncInterval)
	{
		timeSyncRunningTime = 0.0f;
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	playerCharacter = Cast<AMainCharacter>(InPawn);
	if (playerCharacter)
	{
		SetHUDHealth(playerCharacter->GetHealth(), playerCharacter->GetMaxHealth());
	}
}

void AMainPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
}

void AMainPlayerController::SetHUDHealth(float hp, float maxHP)
{
	if (!IsLocalController()) return;
	if(!HUD) HUD = Cast<AMainHUD>(GetHUD());
	
	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetHealth(hp, maxHP);
	}
}

void AMainPlayerController::SetHUDScore(float score)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetScore(score);
	}
}

void AMainPlayerController::SetHUDDeaths(int deaths)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetDeaths(deaths);
	}
}

void AMainPlayerController::SetHUDAmmo(int ammo)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetAmmo(ammo);
	}
}

void AMainPlayerController::SetHUDCarriedAmmo(int carriedAmmo)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetCarriedAmmo(carriedAmmo);
	}
}

void AMainPlayerController::SetHUDMatchTimer(float timer)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		if (timer < 0.0f)
		{
			HUD->GetOverlay()->SetMatchTimer(FString(""));
			return;
		}

		int mins = FMath::FloorToInt(timer / 60.0f);
		int secs = timer - (mins * 60);
		
		HUD->GetOverlay()->SetMatchTimer(FString::Printf(TEXT("%02d : %02d"), mins, secs));
	}
}

void AMainPlayerController::SetHUDMatchInfoTimer(float timer)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && HUD->GetGameInfoOverlay())
	{
		if (timer < 0.0f)
		{
			HUD->GetGameInfoOverlay()->SetWarmupTimer(FString(""));
			return;
		}

		int mins = FMath::FloorToInt(timer / 60.0f);
		int secs = timer - (mins * 60);

		HUD->GetGameInfoOverlay()->SetWarmupTimer(FString::Printf(TEXT("%02d : %02d"), mins, secs));
	}
}

void AMainPlayerController::SetHUDTime()
{
	float timeLeft = 0.0f;
	if (matchState == MatchState::WaitingToStart) timeLeft = warmupTime + (levelStartTime - GetServerTime());
	else if (matchState == MatchState::InProgress) timeLeft = matchTime + warmupTime + (levelStartTime - GetServerTime());
	else if (matchState == MatchState::Cooldown) timeLeft = cooldownTime + matchTime + warmupTime + (levelStartTime - GetServerTime());

	int secLeft = FMath::FloorToInt(timeLeft);

	if (HasAuthority())
	{
		if (!gameMode) gameMode = Cast<AKamikazeStrikeforceGameMode>(UGameplayStatics::GetGameMode(this));
		if (gameMode)
		{
			secLeft = FMath::FloorToInt(gameMode->GetCountDownTime() + gameMode->levelStartTime);
		}
	}
	if (countDown != secLeft)
	{
		if (matchState == MatchState::WaitingToStart)
		{
			SetHUDMatchInfoTimer(timeLeft);
		}
		else if (matchState == MatchState::InProgress)
		{
			SetHUDMatchTimer(timeLeft);
		}
		else if (matchState == MatchState::Cooldown)
		{
			SetHUDMatchInfoTimer(timeLeft);
		}
	}
	countDown = secLeft;
}

void AMainPlayerController::ServerRequestServerTime_Implementation(float timeOfClientReq)
{
	float serverTime = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(timeOfClientReq, serverTime);
}

void AMainPlayerController::ClientReportServerTime_Implementation(float timeOfClientReq, float timeServerReceivedClientReq)
{
	float roundTripTime = GetWorld()->GetTimeSeconds() - timeOfClientReq;

	float currentServerTime = timeServerReceivedClientReq + (roundTripTime * 0.5f);

	serverClientDelta = currentServerTime - GetWorld()->GetTimeSeconds();
}

float AMainPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + serverClientDelta;
}

void AMainPlayerController::OnMatchStateSet(FName state)
{
	matchState = state;

	if (matchState == MatchState::InProgress)
	{
		HandleMatchStarted();
	}
	else if (matchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMainPlayerController::OnRep_MatchState()
{
	if (matchState == MatchState::InProgress)
	{
		HandleMatchStarted();
	}
	else if (matchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void AMainPlayerController::HandleMatchStarted()
{
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
	if (HUD)
	{
		HUD->AddOverlay();
		if (HUD->GetGameInfoOverlay())
		{
			HUD->GetGameInfoOverlay()->SetVisibility(ESlateVisibility::Hidden);	
		}
	}
}

void AMainPlayerController::HandleCooldown()
{
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
	if (HUD)
	{
		HUD->GetOverlay()->RemoveFromParent();
		if (HUD->GetGameInfoOverlay())
		{
			HUD->GetGameInfoOverlay()->SetVisibility(ESlateVisibility::Visible);
		}
	}

	AMainCharacter* character = Cast<AMainCharacter>(GetPawn());
	if (character && character->GetCombat())
	{
		character->disableGameplay = true;
		character->GetCombat()->FirePressed(false);
	}
}

void AMainPlayerController::ServerCheckMatchState_Implementation()
{
	AKamikazeStrikeforceGameMode* _gameMode = Cast<AKamikazeStrikeforceGameMode>(UGameplayStatics::GetGameMode(this));
	if (_gameMode)
	{
		warmupTime = _gameMode->warmupTime;
		matchTime = _gameMode->matchTime;
		levelStartTime = _gameMode->levelStartTime;
		matchState = _gameMode->GetMatchState();
		cooldownTime = _gameMode->cooldownTime;

		ClientJoin(matchState, warmupTime, matchTime, levelStartTime, cooldownTime);
	}
}

void AMainPlayerController::ClientJoin_Implementation(FName _state, float _warmupTime, float _matchTime, float _startTtime, float _cooldownTime)
{
	warmupTime = _warmupTime;
	matchTime = _matchTime;
	levelStartTime = _startTtime;
	matchState = _state;
	cooldownTime = _cooldownTime;
	OnMatchStateSet(matchState);

	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
	if (HUD && matchState == MatchState::WaitingToStart)
	{
		HUD->AddGameInfoOverlay();
	}
}
