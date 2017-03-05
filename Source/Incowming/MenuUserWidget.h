// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Array.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "CoreMisc.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "SharedPointer.h"
#include "MenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class INCOWMING_API UMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	FSocket* Socket;
	FString address;
	FString MServerAddress = "174.94.86.192";
	//FString MServerAddress = "127.0.0.1";
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu Variables")
		TArray<FString> listeServerNames;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu Variables")
		TArray<FString> listeServerIp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu Variables")
		TArray<FString> listeServerMaxPlayers;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu Variables")
		TArray<FString> listeServerNbPlayers;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu Variables")
		FString page;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu Variables")
		FString selectedButtonIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu Variables")
		int nbServers;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu Variables")
		FString myServerName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu Variables")
		FString myServerMaxPlayers = "2";

	UFUNCTION(BlueprintCallable, Category = "Menu Functions")
		void updateListeServers();

	UFUNCTION(BlueprintCallable, Category = "Menu Functions")
		void StartServer();

	void connectToMasterServer();
	void getServersInfos();
};
