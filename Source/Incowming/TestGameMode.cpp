// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Incowming.h"
#include "TestGameMode.h"
#include "CannonCharacter.h"
#include "IncowmingGameState.h"

ATestGameMode::ATestGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_CannonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	GameStateClass = AIncowmingGameState::StaticClass();
}

void ATestGameMode::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	int32 sent = 0;
	int32 read = 0;
	uint8 data = 0;
	TArray<uint8> ReceivedData;
	ReceivedData.Init(data, 2);
	myIP = Response->GetContentAsString();

	//Prepare the data about the maximum number of players allowed and send it
	TCHAR *myIpChar = myIP.GetCharArray().GetData();
	int32 size3 = FCString::Strlen(myIpChar);

	Socket->Send((uint8*)TCHAR_TO_UTF8(myIpChar), size3, sent);
	//Wait for the master server to give the go for the first information
	Socket->Recv(ReceivedData.GetData(), 2, read);
}

void ATestGameMode::updateNbPlayers()
{
	uint32 awaitingDataSize = 0;
	int32 sent = 0;
	uint8 data = 0;
	TArray<uint8> ReceivedData;
	ReceivedData.Init(data, 2);

	if (!Socket->HasPendingData(awaitingDataSize))
	{
		FString nbPlayerFstring = FString::FromInt(nbPlayers);
		TCHAR *nbPlayerChar = nbPlayerFstring.GetCharArray().GetData();
		int32 size4 = FCString::Strlen(nbPlayerChar);
		Socket->Send((uint8*)TCHAR_TO_UTF8(nbPlayerChar), size4, sent);
	}

	Socket->Recv(ReceivedData.GetData(), 2, sent);
}

bool ATestGameMode::getMyPublicIp()
{
	Http = &FHttpModule::Get();

	if (!Http)
	{
		return false;
	}

	if (!Http->IsHttpEnabled())
	{
		return false;
	}
	FString TargetHost = "http://api.ipify.org";
	TSharedRef <IHttpRequest> Request = Http->CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL(TargetHost);
	Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
	Request->SetHeader("Content-Type", "text/html");
	Request->OnProcessRequestComplete().BindUObject(this, &ATestGameMode::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		return false;
	}

	return true;
}

void ATestGameMode::ConnectSocket()
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
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
			Socket->Recv(ReceivedData.GetData(), 2 , read);

			if (!Socket->HasPendingData(awaitingDataSize))
			{
				//Prepare the data about what type of request it is and send it
				FString typeOfSocket = TEXT("Server");
				TCHAR *typeOfSocketChar = typeOfSocket.GetCharArray().GetData();
				int32 size1 = FCString::Strlen(typeOfSocketChar);
				Socket->Send((uint8*)TCHAR_TO_UTF8(typeOfSocketChar), size1, sent);
			}


			//Wait for the master server to give the go for the first information
			Socket->Recv(ReceivedData.GetData(), 2, read);

			if (!Socket->HasPendingData(awaitingDataSize))
			{
				//Prepare the data about the server name and send it
				TCHAR *serverNameChar = serverName.GetCharArray().GetData();
				int32 size2 = FCString::Strlen(serverNameChar);
				Socket->Send((uint8*)TCHAR_TO_UTF8(serverNameChar), size2, sent);
			}

			//Wait for the master server to give the go for the first information
			Socket->Recv(ReceivedData.GetData(), 2, read);
			
			if (!Socket->HasPendingData(awaitingDataSize))
			{
				//Prepare the data about the maximum number of players allowed and send it
				TCHAR *maxPlayersChar = maxPlayers.GetCharArray().GetData();
				int32 size3 = FCString::Strlen(maxPlayersChar);

				Socket->Send((uint8*)TCHAR_TO_UTF8(maxPlayersChar), size3, sent);
			}
			//Wait for the master server to give the go for the first information
			Socket->Recv(ReceivedData.GetData(), 2, read);

			if (!Socket->HasPendingData(awaitingDataSize))
			{
				getMyPublicIp();
			}

		}
	}
}

void ATestGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr < const FUniqueNetId > & UniqueId, FString & ErrorMessage)
{
	AIncowmingGameState *GameState = GetWorld()->GetGameState<AIncowmingGameState>();
	// Reject the connexion if the game has already started
	if ((GameState->GetCurrentMatchState() != EMatchStateEnum::WAITING) ^ (GameState->GetCurrentMatchState() == EMatchStateEnum::INFINITEREADY)) {
		ErrorMessage = "Server refused a connection : the match is already started.";
		UE_LOG(LogTemp, Warning, TEXT("Server refused a connection: the match is already started."));
		return;
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

int ATestGameMode::getFreeSlotInPlayerList()
{
	if (playerList.Num() == 0)
		return 0;
	else
	{
		for (int i = 0; i < nbPlayersMax; i++)
		{
			if (playerList[i] == -1)
				return i;
		}
		return -1;
	}
}

void ATestGameMode::addPlayer(int id)
{
	playerList.Insert(id, id - 1);
}

void ATestGameMode::PostLogin(APlayerController * NewPlayer)
{
	AIncowmingGameState *GameState = GetWorld()->GetGameState<AIncowmingGameState>();
	int playerId = -1;
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		nbPlayers++;
		updateNbPlayers();
		playerId = getFreeSlotInPlayerList() + 1;
		FString playerName = FString::FromInt(playerId);
		addPlayer(playerId);
		NewPlayer->SetName(playerName);
	}

	Super::PostLogin(NewPlayer);

	ACannonCharacter * cannonCharacter = Cast<ACannonCharacter>(NewPlayer->GetCharacter());
	if (cannonCharacter) 
	{
		GameState->UpdateNbPlayers();
		GameState->UpdateNbPlayersReady();
		FLinearColor playerColors[] = {
			FLinearColor(1,		0.5,	0),		// Orange
			FLinearColor(0,		1,		1),		// Cyan
			FLinearColor(1,		0,		1),		// Magenta
			FLinearColor(0,		0,		1),		// Bleu 255
			FLinearColor(1,		1,		0),		// Jaune banane
			FLinearColor(0.8,	0.8,	0.8)	// Gris Pale
		};

		FLinearColor color = playerColors[playerId-1];
		cannonCharacter->SetPlayerColor(color);
		lastIndex++;
	}
}

void ATestGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		nbPlayers = 0;
		FString file = "./ServerConfig.ini";
		FString fileData = "Server_Name=Useless boar|Max_Players=4|IP_address=174.94.86.192";
		FFileHelper::LoadFileToString(fileData, *file);
		TArray<FString> eachSettings = {};
		fileData.ParseIntoArray(eachSettings, TEXT("|"), false);

		TArray<FString> serverNameDatas = {};
		TArray<FString> MaxPlayersDatas = {};
		TArray<FString> IPAddDatas = {};
		eachSettings[0].ParseIntoArray(serverNameDatas, TEXT("="), false);
		eachSettings[1].ParseIntoArray(MaxPlayersDatas, TEXT("="), false);
		eachSettings[2].ParseIntoArray(IPAddDatas, TEXT("="), false);

		serverName = serverNameDatas[1];
		maxPlayers = MaxPlayersDatas[1];
		address = IPAddDatas[1];

		nbPlayersMax = FCString::Atoi(*maxPlayers);

		// Zero to bypass Infinite Players Max
		if (nbPlayersMax == 0) {
			UE_LOG(LogTemp, Warning, TEXT("INFINITE READY MODE"));
			maxPlayers = "100";
			nbPlayersMax = FCString::Atoi(*maxPlayers);
		}

		ConnectSocket();
		for (int i = 0; i < nbPlayersMax; i++)
		{
			playerList.Insert(-1, i);
		}
	}
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ATestGameMode::removePlayer(int id)
{
	playerList.Insert(-1, id - 1);
}

void ATestGameMode::Logout(AController * Exiting)
{
	AIncowmingGameState *GameState = GetWorld()->GetGameState<AIncowmingGameState>();
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		nbPlayers--;
		GameState->UpdateNbPlayers();
		updateNbPlayers();
		APlayerState* ThePlayerState = Exiting->PlayerState;
		FString PlayerName = ThePlayerState->PlayerName;
		removePlayer(FCString::Atoi(*PlayerName));
	}

	if (nbPlayers == 0)
		GameState->SetCurrentMatchState(EMatchStateEnum::WAITING);

	Super::Logout(Exiting);
}