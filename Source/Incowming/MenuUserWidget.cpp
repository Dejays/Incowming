// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "MenuUserWidget.h"

void UMenuUserWidget::getServersInfos()
{
	int32 sent = 0;
	int32 read = 0;

	uint32 awaitingDataSize = 0;
	FString emptyString = TEXT("ab");
	TCHAR *EmptyChar = emptyString.GetCharArray().GetData();
	int32 size1 = FCString::Strlen(EmptyChar);

	
	Socket->Send((uint8*)TCHAR_TO_UTF8(EmptyChar), size1, sent);

	uint8 dataNbServers = 0;
	uint8 ReceivedNbServers[1000];
	Socket->Recv(ReceivedNbServers, 1000, read);
	char ansiiNbServers[5000];

	memcpy(ansiiNbServers, ReceivedNbServers, read);
	ansiiNbServers[read] = 0;

	UE_LOG(ServerLog, Warning, TEXT("Nb Servers: %s"), ANSI_TO_TCHAR(ansiiNbServers));

	nbServers = FCString::Atoi(ANSI_TO_TCHAR(ansiiNbServers));

	if (!Socket->HasPendingData(awaitingDataSize))
	{
		Socket->Send((uint8*)TCHAR_TO_UTF8(EmptyChar), size1, sent);
	}

	if (nbServers > 0)
	{
		for (int i = 0; i < nbServers; i++)
		{
			uint8 ReceivedServerName[1000];
			Socket->Recv(ReceivedServerName, 1000, read);
			char ansiiServerName[5000];
			memcpy(ansiiServerName, ReceivedServerName, read);
			ansiiServerName[read] = 0;
			FString ServerName = ANSI_TO_TCHAR(ansiiServerName);
			listeServerNames.Add(ServerName);
			UE_LOG(ServerLog, Warning, TEXT("Nb Servers: %s"), ANSI_TO_TCHAR(ansiiServerName));
			if (!Socket->HasPendingData(awaitingDataSize))
			{
				Socket->Send((uint8*)TCHAR_TO_UTF8(EmptyChar), size1, sent);
			}

			uint8 ReceivedServerIP[1000];
			Socket->Recv(ReceivedServerIP, 1000, read);
			char ansiiServerIP[5000];
			memcpy(ansiiServerIP, ReceivedServerIP, read);
			ansiiServerIP[read] = 0;
			FString ServerIp = ANSI_TO_TCHAR(ansiiServerIP);
			UE_LOG(ServerLog, Warning, TEXT("Nb Servers: %s"), ANSI_TO_TCHAR(ansiiServerIP));
			listeServerIp.Add(ServerIp);

			if (!Socket->HasPendingData(awaitingDataSize))
			{
				Socket->Send((uint8*)TCHAR_TO_UTF8(EmptyChar), size1, sent);
			}

			uint8 ReceivedServerNbPlayers[1000];
			Socket->Recv(ReceivedServerNbPlayers, 1000, read);
			char ansiiServerNbPlayers[5000];
			memcpy(ansiiServerNbPlayers, ReceivedServerNbPlayers, read);
			ansiiServerNbPlayers[read] = 0;
			FString ServerNbPlayers = ANSI_TO_TCHAR(ansiiServerNbPlayers);
			UE_LOG(ServerLog, Warning, TEXT("Nb Servers: %s"), ANSI_TO_TCHAR(ansiiServerNbPlayers));
			listeServerNbPlayers.Add(ServerNbPlayers);

			if (!Socket->HasPendingData(awaitingDataSize))
			{
				Socket->Send((uint8*)TCHAR_TO_UTF8(EmptyChar), size1, sent);
			}

			uint8 ReceivedServerMaxPlayers[1000];
			Socket->Recv(ReceivedServerMaxPlayers, 1000, read);
			char ansiiServerMaxPlayers[5000];
			memcpy(ansiiServerMaxPlayers, ReceivedServerMaxPlayers, read);
			ansiiServerMaxPlayers[read] = 0;
			FString ServerMaxPlayers = ANSI_TO_TCHAR(ansiiServerMaxPlayers);
			listeServerMaxPlayers.Add(ServerMaxPlayers);

			if (!Socket->HasPendingData(awaitingDataSize))
			{
				Socket->Send((uint8*)TCHAR_TO_UTF8(EmptyChar), size1, sent);
			}
		}
	}
	Socket->Close();
}

void UMenuUserWidget::connectToMasterServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	int32 port = 7777;
	FIPv4Address ip;
	FIPv4Address::Parse(address, ip);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(port);

	if (Socket->Connect(*addr))
	{
		int32 sent = 0;
		int32 read = 0;
		uint8 data = 0;
		uint32 awaitingDataSize = 0;
		TArray<uint8> ReceivedData;
		ReceivedData.Init(data, 2);

		//Wait for the master server to give the go for the first information
		Socket->Recv(ReceivedData.GetData(), 2, read);

		if (!Socket->HasPendingData(awaitingDataSize))
		{
			//Prepare the data about what type of request it is and send it
			FString typeOfSocket = TEXT("Player");
			TCHAR *typeOfSocketChar = typeOfSocket.GetCharArray().GetData();
			int32 size1 = FCString::Strlen(typeOfSocketChar);
			Socket->Send((uint8*)TCHAR_TO_UTF8(typeOfSocketChar), size1, sent);
		}

		Socket->Recv(ReceivedData.GetData(), 2, read);
		if (!Socket->HasPendingData(awaitingDataSize))
		{
			getServersInfos();
		}
	}
}

void UMenuUserWidget::updateListeServers()
{
	GConfig->GetString(
		TEXT("Server.ServerInfo"),
		TEXT("IP_address"),
		address,
		GGameIni
	);
	listeServerNames.Empty();
	listeServerIp.Empty();
	listeServerMaxPlayers.Empty();
	listeServerNbPlayers.Empty();
	connectToMasterServer();

}

void UMenuUserWidget::StartServer()
{
	FString SaveDirectory = FString(".");
	FString FileName = FString("ServerConfig.ini");
	FString TextToSave = FString(TEXT("Server_Name=")) + myServerName + FString(TEXT("|Max_Players=")) + myServerMaxPlayers + FString(TEXT("|IP_address=")) + MServerAddress;
	FString AbsoluteFilePath = SaveDirectory + "/" + FileName;
	FFileHelper::SaveStringToFile(TextToSave, *AbsoluteFilePath);

	FPlatformProcess::CreateProc(TEXT("IncowmingServer.exe"), TEXT("-log"), true, false, false, nullptr, 0, nullptr, nullptr);
}