// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

bool UMenu::Initialize()
{
	if(!Super::Initialize())
		return false;

	if (hostButton)
		hostButton->OnClicked.AddDynamic(this, &UMenu::OnHostButtonClicked);

	if (joinButton)
		joinButton->OnClicked.AddDynamic(this, &UMenu::OnJoinButtonClicked);

	return true;
}

void UMenu::InitializeMenu(FString _pathToLobby, int32 _numPublicConnections, FString _matchType)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	auto world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GI = GetGameInstance();
	if (GI)
		multiplayerSubsystem = GI->GetSubsystem<UMultiplayerSessionsSubsystem>();

	numPublicConnections = _numPublicConnections;
	matchType = _matchType;
	pathToLobby = _pathToLobby;

	if (multiplayerSubsystem)
	{
		multiplayerSubsystem->multiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
		multiplayerSubsystem->multiplayerOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSessions);
		multiplayerSubsystem->multiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
		multiplayerSubsystem->multiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
		multiplayerSubsystem->multiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
	}
}

void UMenu::NativeDestruct()
{
	RemoveMenu();
	Super::NativeDestruct();
}

void UMenu::OnHostButtonClicked()
{
	hostButton->SetIsEnabled(false);
	if (multiplayerSubsystem)
	{
		multiplayerSubsystem->CreateSession(numPublicConnections, matchType);
	}
}

void UMenu::OnJoinButtonClicked()
{
	joinButton->SetIsEnabled(false);
	if (multiplayerSubsystem)
	{
		multiplayerSubsystem->FindSession(10000); //Set to high number because we are using place holder dev_app_ID for steam
	}
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		auto world = GetWorld();
		if (world)
		{
			world->ServerTravel(pathToLobby + "?listen");
		}
	}
	else
	{
		hostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& searchResults, bool bWasSuccessful)
{
	if (!multiplayerSubsystem) return;

	if (bWasSuccessful)
	{
		for (auto result : searchResults)
		{
			FString id = result.GetSessionIdStr();
			FString user = result.Session.OwningUserName;

			FString _matchType;
			result.Session.SessionSettings.Get(FName("MatchType"), _matchType);			

			if (_matchType == matchType)
			{
				multiplayerSubsystem->JoinSession(result);
				return;
			}
		}
	}
	
	if(!bWasSuccessful || searchResults.Num() == 0)
	{
		joinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	if (result != EOnJoinSessionCompleteResult::Success)
	{
		joinButton->SetIsEnabled(true);
		return;
	}

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem)
	{
		IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface();
		if (!onlineSessionInterface.IsValid())
		{
			return;
		}

		FString address;
		if (onlineSessionInterface->GetResolvedConnectString(NAME_GameSession, address))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Connecting with: %s"), *address));
			}

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnStartSession(bool bWasSuccessful)
{

}

void UMenu::OnDestroySession(bool bWasSuccessful)
{

}


void UMenu::RemoveMenu()
{
	RemoveFromParent();
	auto world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}
}