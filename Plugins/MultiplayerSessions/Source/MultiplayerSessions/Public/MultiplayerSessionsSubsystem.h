// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& searchResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);

UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	void CreateSession(int32 numPublicConnections, FString matchType);
	void FindSession(int32 maxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& sessionResults);
	void StartSession();
	void DestroySession();

	FMultiplayerOnCreateSessionComplete multiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete multiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete multiplayerOnJoinSessionComplete;
	FMultiplayerOnStartSessionComplete multiplayerOnStartSessionComplete;
	FMultiplayerOnDestroySessionComplete multiplayerOnDestroySessionComplete;

protected:

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr sessionInterface;

	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FDelegateHandle createSessionCompleteDelegate_Handle;
	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;
	FDelegateHandle findSessionsCompleteDelegate_Handle;
	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;
	FDelegateHandle joinSessionCompleteDelegate_Handle;
	FOnStartSessionCompleteDelegate startSessionCompleteDelegate;
	FDelegateHandle startSessionCompleteDelegate_Handle;
	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;
	FDelegateHandle destroySessionCompleteDelegate_Handle;

	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	//TSharedPtr<FOnlineSessionSettings> sessionSettings;

	bool bCreateSessionOnDestroy = false;
	int32 lastNumPublicConnections;
	FString lastMatchType;
};
