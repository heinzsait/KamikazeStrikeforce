// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UButton;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void InitializeMenu(FString _pathToLobby = FString("/Game/Maps/Lobby"), int32 _numPublicConnections = 4, FString _matchType = FString("FreeForAll"));


protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;


	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& searchResults, bool bWasSuccessful);

	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

private:

	UPROPERTY(meta = (BindWidget))
	UButton* hostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* joinButton;

	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();

	class UMultiplayerSessionsSubsystem* multiplayerSubsystem;

	int32 numPublicConnections;
	FString matchType;
	FString pathToLobby;

	UFUNCTION(BlueprintCallable)
	void RemoveMenu();
};
