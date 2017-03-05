// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include <string>
#include "Http.h"
#include <stdio.h>
#include <stdlib.h>
#include "SharedPointer.h"
#include "TestGameMode.generated.h"

/**
*
*/
UCLASS()
class INCOWMING_API ATestGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	TArray<int> playerList;
	FSocket* Socket;
	int nbPlayers = 0;
	int nbPlayersMax = 0;
	FString serverName;
	FString maxPlayers;
	FString address;
	FString myIP;
	int lastIndex = 0;
public:
	ATestGameMode();
	FHttpModule* Http;
	void updateNbPlayers();
	void ConnectSocket();
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr < const FUniqueNetId > & UniqueId, FString & ErrorMessage) override;
	virtual void PostLogin(APlayerController * NewPlayer) override;
	virtual void InitGame(const FString & MapName,
		const FString & Options,
		FString & ErrorMessage) override;
	virtual void Logout(AController * Exiting) override;
	bool getMyPublicIp();
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	int getNbPlayers() { return nbPlayers; }
	int getFreeSlotInPlayerList();
	void addPlayer(int id);
	void removePlayer(int id);
	int getNbPlayersMax() { return nbPlayersMax; }
	void CloseSocket() { Socket->Close(); }
};
