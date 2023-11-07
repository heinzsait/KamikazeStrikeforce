// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "KamikazeStrikeforce/HUD/MainHUD.h"
#include "KamikazeStrikeforce/HUD/CharacterOverlay.h"
#include "KamikazeStrikeforce/HUD/GameInfoOverlay.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/Components/CombatComponent.h"
#include "KamikazeStrikeforce/GameMode/KamikazeStrikeforceGameMode.h"
#include "KamikazeStrikeforce/PlayerState/MainPlayerState.h"
#include "KamikazeStrikeforce/GameState/MainGameState.h"
#include "KamikazeStrikeforce/Misc/MainSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<AMainHUD>(GetHUD());	
	//if(HUD) HUD->AddGameInfoOverlay();

	ServerCheckMatchState();

	if (IsLocalController())
		LoadGameFromSave();
}

void AMainPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPlayerController, matchState);
	DOREPLIFETIME(AMainPlayerController, playerAvatar);
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);

	CheckPing(DeltaTime);

	if (infoNotSet)
	{
		if (!HasAuthority())
		{
			if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
			if (HUD && matchState == MatchState::WaitingToStart)
			{
				if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("adding GameInfoOverlay in Tick...")));
				HUD->AddGameInfoOverlay();

				infoNotSet = false;
			}
		}
		else
		{
			AKamikazeStrikeforceGameMode* _gameMode = Cast<AKamikazeStrikeforceGameMode>(UGameplayStatics::GetGameMode(this));
			if (_gameMode)
			{
				warmupTime = _gameMode->warmupTime;
				matchTime = _gameMode->matchTime;
				levelStartTime = _gameMode->levelStartTime;
				matchState = _gameMode->GetMatchState();
				cooldownTime = _gameMode->cooldownTime;

				if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
				if (HUD && matchState == MatchState::WaitingToStart)
				{
					if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("adding GameInfoOverlay in Tick...")));
					HUD->AddGameInfoOverlay();

					infoNotSet = false;
				}
			}
		}
	}
}

void AMainPlayerController::CheckPing(float DeltaTime)
{
	highPingRunningTime += DeltaTime;
	if (highPingRunningTime > checkPingInterval)
	{
		if (!PlayerState) PlayerState = GetPlayerState<AMainPlayerState>();
		if (PlayerState)
		{
			if (PlayerState->GetPingInMilliseconds() > highPingThreshold)
			{
				HighPingWarning();
				highPingAnimRunningTime = 0.0f;
			}
		}
		highPingRunningTime = 0.0f;
	}
	if (HUD && HUD->GetOverlay() && HUD->GetOverlay()->highPingAnim && HUD->GetOverlay()->IsAnimationPlaying(HUD->GetOverlay()->highPingAnim))
	{
		highPingAnimRunningTime += DeltaTime;
		if (highPingAnimRunningTime > highPingDuration)
		{
			StopHighPingWarning();
		}
	}
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
	singleTripTime = roundTripTime * 0.5f;
	float currentServerTime = timeServerReceivedClientReq + singleTripTime;

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
		if(HUD->GetOverlay() == nullptr)
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

			AMainGameState* gameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
			AMainPlayerState* playerState = GetPlayerState<AMainPlayerState>();
			if (gameState && playerState)
			{
				auto topPlayers = gameState->topPlayers;
				FString infoText = FString("");
				if (topPlayers.Num() == 0)
				{
					infoText = FString("No winner :(");
				}
				else if (topPlayers.Num() == 1 && topPlayers[0] == playerState)
				{
					infoText = FString("You are the winner");
				}
				else if (topPlayers.Num() == 1)
				{
					infoText = FString::Printf(TEXT("%s is the winner"), *topPlayers[0]->GetPlayerName());
				}
				else if (topPlayers.Num() > 1)
				{
					infoText = FString("Winners: \n");
					for (auto player : topPlayers)
					{
						infoText.Append(FString::Printf(TEXT("%s, "), *player->GetPlayerName()));
					}
				}
				HUD->GetGameInfoOverlay()->SetInfoText(infoText);
			}
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
	else
		infoNotSet = true;
}

void AMainPlayerController::ClientJoin_Implementation(FName _state, float _warmupTime, float _matchTime, float _startTtime, float _cooldownTime)
{
	warmupTime = _warmupTime;
	matchTime = _matchTime;
	levelStartTime = _startTtime;
	matchState = _state;
	cooldownTime = _cooldownTime;
	OnMatchStateSet(matchState);

	HUD = Cast<AMainHUD>(GetHUD());

	if (HUD && matchState == MatchState::WaitingToStart)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("adding GameInfoOverlay...")));
		HUD->AddGameInfoOverlay();
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Failed to add GameInfoOverlay, No valid HUD")));
		infoNotSet = true;
	}
}

void AMainPlayerController::HighPingWarning()
{
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
	if (HUD && HUD->GetOverlay() && HUD->GetOverlay()->highPingImg && HUD->GetOverlay()->highPingAnim)
	{
		HUD->GetOverlay()->highPingImg->SetOpacity(1.0f);
		HUD->GetOverlay()->PlayAnimation(HUD->GetOverlay()->highPingAnim, 0.0f, 3);
	}
}

void AMainPlayerController::StopHighPingWarning()
{
	if (!HUD) HUD = Cast<AMainHUD>(GetHUD());
	if (HUD && HUD->GetOverlay() && HUD->GetOverlay()->highPingImg && HUD->GetOverlay()->highPingAnim)
	{
		HUD->GetOverlay()->highPingImg->SetOpacity(1.0f);
		if (HUD->GetOverlay()->IsAnimationPlaying(HUD->GetOverlay()->highPingAnim))
			HUD->GetOverlay()->StopAnimation(HUD->GetOverlay()->highPingAnim);
	}
}


void AMainPlayerController::ServerInitAvatar_Implementation(EAvatar _avatar)
{
	MulticastInitAvatar(_avatar);
}

void AMainPlayerController::MulticastInitAvatar_Implementation(EAvatar _avatar)
{
	playerAvatar = _avatar;
}

void AMainPlayerController::LoadGameFromSave()
{
	UMainSaveGame* mainSaveGame = Cast<UMainSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));
	mainSaveGame = Cast<UMainSaveGame>(UGameplayStatics::LoadGameFromSlot("slot0", 0));


	if (HasAuthority())
	{
		if (mainSaveGame)
		{
			playerAvatar = mainSaveGame->playerAvatar;
			//playerAvatar = (EAvatar)FMath::RandRange((int)EAvatar::Manny, (int)EAvatar::MAX - 1);
		}
	}
	else
	{
		if (mainSaveGame)
		{
			ServerInitAvatar(mainSaveGame->playerAvatar);

			//ServerInitAvatar((EAvatar)FMath::RandRange((int)EAvatar::Manny, (int)EAvatar::MAX - 1));
		}
	}
}