// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "TestGameMode.h"
#include "IncowmingGameState.h"
#include "Net/UnrealNetwork.h"
#include "CannonCharacter.h"

AIncowmingGameState::AIncowmingGameState() {
	currentMatchState = EMatchStateEnum::WAITING;
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void AIncowmingGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

EMatchStateEnum AIncowmingGameState::GetCurrentMatchState()
{
	return currentMatchState;
}

void AIncowmingGameState::SetCurrentMatchState(EMatchStateEnum state)
{
	currentMatchState = state;
}

int AIncowmingGameState::GetNbPlayersMax() {
	return nbPlayersMax;
}

int AIncowmingGameState::GetNbPlayers() {
	return nbPlayers;
}

void AIncowmingGameState::UpdateNbPlayers_Implementation() {
	UWorld * world = GetWorld();
	int tmpNbPlayers = 0;
	for (TActorIterator<ACannonCharacter> ActorItr(world); ActorItr; ++ActorItr)
	{
		tmpNbPlayers++;
	}
	nbPlayers = tmpNbPlayers;
	UE_LOG(LogTemp, Warning, TEXT("GameState NbPlayers : %d"), nbPlayers);
}
bool AIncowmingGameState::UpdateNbPlayers_Validate() { return true; }

int AIncowmingGameState::GetNbPlayersReady() {
	return nbPlayersReady;
}

void AIncowmingGameState::UpdateNbPlayersReady_Implementation() {
	UWorld * world = GetWorld();
	ATestGameMode* GameMode = world->GetAuthGameMode<ATestGameMode>();

	int tmpNbPlayersReady = 0;
	nbPlayersMax = GameMode->getNbPlayersMax();
	
	for (TActorIterator<ACannonCharacter> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (ActorItr->bReady) {
			tmpNbPlayersReady++;
		}
	}
	nbPlayersReady = tmpNbPlayersReady;
	UE_LOG(LogTemp, Warning, TEXT("GameState NbPlayers : %d, NbPlayersReady : %d / GameMode NbPlayersMax : %d"), nbPlayers, nbPlayersReady, nbPlayersMax);

		
}
bool AIncowmingGameState::UpdateNbPlayersReady_Validate() { return true; }


void AIncowmingGameState::UpdateNbPlayersDead_Implementation(FName deadPlayerName) {
	int tmpNbPlayersDead = 0;
	UWorld * world = GetWorld();
	

	for (TActorIterator<ACannonCharacter> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (ActorItr->bDead) {
			tmpNbPlayersDead++;
			ActorItr->ClientYouLose();
		}
		else {
			ActorItr->ClientMessageReceived(deadPlayerName);
		}
	}
	nbPlayersDead = tmpNbPlayersDead;
	UE_LOG(LogTemp, Warning, TEXT("GameState NbPlayers : %d, NbPlayersDead : %d"), nbPlayers, nbPlayersDead);
	if (nbPlayersDead == (nbPlayers-1))
		EndGame();
}
bool AIncowmingGameState::UpdateNbPlayersDead_Validate(FName deadPlayerName) { return true; }


void AIncowmingGameState::StartGame_Implementation(EMatchStateEnum state) {
	// Ne pas commencer une partie déja commencée
	if (currentMatchState == EMatchStateEnum::STARTED) {
		return;
	}

	if (currentMatchState != state)
		currentMatchState = state;

	UWorld * world = GetWorld();
	for (TActorIterator<ACannonCharacter> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (!ActorItr->gameStarted)
			ActorItr->HandleGameStarted();
	}
}
bool AIncowmingGameState::StartGame_Validate(EMatchStateEnum state) { return true; }

void AIncowmingGameState::RestartGame()
{
	ATestGameMode* GameMode = GetWorld()->GetAuthGameMode<ATestGameMode>();
	GameMode->CloseSocket();
	GameMode->RestartGame();
}

void AIncowmingGameState::EndGame_Implementation() {
	currentMatchState = EMatchStateEnum::ENDED;
	UWorld * world = GetWorld();
	for (TActorIterator<ACannonCharacter> ActorItr(world); ActorItr; ++ActorItr)
	{
		if (!ActorItr->bDead) {
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ActorItr->ClientYouWin();
		}
	}
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AIncowmingGameState::RestartGame, 5.0f, false);
}
bool AIncowmingGameState::EndGame_Validate() { return true; }


void AIncowmingGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIncowmingGameState, currentMatchState);
	DOREPLIFETIME(AIncowmingGameState, nbPlayers);
	DOREPLIFETIME(AIncowmingGameState, nbPlayersReady);
	DOREPLIFETIME(AIncowmingGameState, nbPlayersDead);
}