// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CannonCharacter.h"
#include "CooldownTimer.generated.h"

/**
 * 
 */
UCLASS()
class INCOWMING_API ACooldownTimer : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Cooldown;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float Timer;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float TimeRemaining;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool Activated;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//The canonCharacter who spawned the timer.
	ACannonCharacter *CooldownTimerOwner;

public:
	// Constructor
	ACooldownTimer();

	UFUNCTION(NetMulticast, Reliable)
	void SetCooldownTimerOwner(ACannonCharacter* theOwner);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
	void MulticastCooldownActive();

	// Sets Cooldown value
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownTimer")
	void Initialize(const float CooldownUnits, ACannonCharacter* theOwner);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	// Reset Cooldown Timer
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Incowming|CooldownTimer")
	void ResetCooldown();

	// Reduces Cooldown Timer by percent 
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownTimer")
	void ReduceCooldown(float percent);

	// Calculate the time when the cooldown is active
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownTimer")
	void CalculateTimer();

	// Returns the remaining time of the Cooldown
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownTimer")
	float GetDisplayedTimer();

	// Returns if the cooldown is inactive
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownTimer")
	bool IsInactive();
	
	// Getter and Setter of Cooldown (use only in code)
	inline float GetCooldown() { return Cooldown; }
	inline void SetCooldown(float value) { Cooldown = value; }	

	// Getter and Setter of Timer (use only in code)
	inline float GetTimer() { return Timer; }
	inline void SetTimer(float value) { Timer = value; }
};
