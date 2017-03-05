// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "IncowmingGameState.generated.h"

UENUM(BlueprintType)
enum class EMatchStateEnum : uint8 { 
	WAITING = 0 UMETA(DisplayName="Waiting"),
	STARTED = 1 UMETA(DisplayName="Started"),
	ENDED = 2 UMETA(DisplayName = "Ended"),
	INFINITEREADY = 3 UMETA(DisplayName = "Infinite")
};


/**
 * 
 */
UCLASS()
class INCOWMING_API AIncowmingGameState : public AGameState
{
	GENERATED_BODY()

public:
	AIncowmingGameState();

	UPROPERTY(Replicated)
		int nbPlayers;

	UPROPERTY(Replicated)
		int nbPlayersReady;

	UPROPERTY(Replicated)
		int nbPlayersDead;

	UFUNCTION(BlueprintCallable, Category = "IncowmingGameState")
		EMatchStateEnum GetCurrentMatchState();

	UFUNCTION(BlueprintCallable, Category = "IncowmingGameState")
		void SetCurrentMatchState(EMatchStateEnum state);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingGameState")
		void StartGame(EMatchStateEnum state);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingGameState")
		void EndGame();

	UFUNCTION(BlueprintCallable, Category = "IncowmingGameState")
		int GetNbPlayers();

	UFUNCTION(BlueprintCallable, Category = "IncowmingGameState")
		int GetNbPlayersMax();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingGameState")
		void UpdateNbPlayers();

	UFUNCTION(BlueprintCallable, Category = "IncowmingGameState")
		int GetNbPlayersReady();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingGameState")
		void UpdateNbPlayersReady();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingGameState")
		void UpdateNbPlayersDead(FName deadPlayerName);

	virtual void Tick(float DeltaSeconds) override;
	float tickTimer;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

private:

	UPROPERTY(Replicated)
		EMatchStateEnum currentMatchState;

	int nbPlayersMax;
	
	void RestartGame();
};
