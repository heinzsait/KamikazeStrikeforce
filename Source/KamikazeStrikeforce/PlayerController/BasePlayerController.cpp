// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "KamikazeStrikeforce/HUD/BaseHUD.h"
#include "KamikazeStrikeforce/HUD/CharacterOverlay.h"
#include "KamikazeStrikeforce/HUD/GameInfoOverlay.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"
#include "KamikazeStrikeforce/Components/CombatComponent.h"
#include "KamikazeStrikeforce/GameMode/KamikazeStrikeforceGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ABaseHUD>(GetHUD());	
	//if(HUD) HUD->AddGameInfoOverlay();
	ServerCheckMatchState();
}

void ABasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerController, matchState);
}

void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);
}

void ABasePlayerController::CheckTimeSync(float DeltaTime)
{
	timeSyncRunningTime += DeltaTime;
	if (IsLocalController() && timeSyncRunningTime > timeSyncInterval)
	{
		timeSyncRunningTime = 0.0f;
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	playerCharacter = Cast<ABaseCharacter>(InPawn);
	if (playerCharacter)
	{
		SetHUDHealth(playerCharacter->GetHealth(), playerCharacter->GetMaxHealth());
	}
}

void ABasePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
}

void ABasePlayerController::SetHUDHealth(float hp, float maxHP)
{
	if (!IsLocalController()) return;
	if(!HUD) HUD = Cast<ABaseHUD>(GetHUD());
	
	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetHealth(hp, maxHP);
	}
}

void ABasePlayerController::SetHUDScore(float score)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetScore(score);
	}
}

void ABasePlayerController::SetHUDDeaths(int deaths)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetDeaths(deaths);
	}
}

void ABasePlayerController::SetHUDAmmo(int ammo)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetAmmo(ammo);
	}
}

void ABasePlayerController::SetHUDCarriedAmmo(int carriedAmmo)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

	if (HUD && HUD->GetOverlay())
	{
		HUD->GetOverlay()->SetCarriedAmmo(carriedAmmo);
	}
}

void ABasePlayerController::SetHUDMatchTimer(float timer)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

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

void ABasePlayerController::SetHUDMatchInfoTimer(float timer)
{
	if (!IsLocalController()) return;
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());

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

void ABasePlayerController::SetHUDTime()
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

void ABasePlayerController::ServerRequestServerTime_Implementation(float timeOfClientReq)
{
	float serverTime = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(timeOfClientReq, serverTime);
}

void ABasePlayerController::ClientReportServerTime_Implementation(float timeOfClientReq, float timeServerReceivedClientReq)
{
	float roundTripTime = GetWorld()->GetTimeSeconds() - timeOfClientReq;

	float currentServerTime = timeServerReceivedClientReq + (roundTripTime * 0.5f);

	serverClientDelta = currentServerTime - GetWorld()->GetTimeSeconds();
}

float ABasePlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + serverClientDelta;
}

void ABasePlayerController::OnMatchStateSet(FName state)
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

void ABasePlayerController::OnRep_MatchState()
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


void ABasePlayerController::HandleMatchStarted()
{
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->AddOverlay();
		if (HUD->GetGameInfoOverlay())
		{
			HUD->GetGameInfoOverlay()->SetVisibility(ESlateVisibility::Hidden);	
		}
	}
}

void ABasePlayerController::HandleCooldown()
{
	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->GetOverlay()->RemoveFromParent();
		if (HUD->GetGameInfoOverlay())
		{
			HUD->GetGameInfoOverlay()->SetVisibility(ESlateVisibility::Visible);
		}
	}

	ABaseCharacter* character = Cast<ABaseCharacter>(GetPawn());
	if (character && character->GetCombat())
	{
		character->disableGameplay = true;
		character->GetCombat()->FirePressed(false);
	}
}

void ABasePlayerController::ServerCheckMatchState_Implementation()
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

void ABasePlayerController::ClientJoin_Implementation(FName _state, float _warmupTime, float _matchTime, float _startTtime, float _cooldownTime)
{
	warmupTime = _warmupTime;
	matchTime = _matchTime;
	levelStartTime = _startTtime;
	matchState = _state;
	cooldownTime = _cooldownTime;
	OnMatchStateSet(matchState);

	if (!HUD) HUD = Cast<ABaseHUD>(GetHUD());
	if (HUD && matchState == MatchState::WaitingToStart)
	{
		HUD->AddGameInfoOverlay();
	}
}
