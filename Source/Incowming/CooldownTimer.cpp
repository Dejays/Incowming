// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "CooldownTimer.h"
#include "Net/UnrealNetwork.h"


ACooldownTimer::ACooldownTimer()
{
	this->bReplicates = true;
	this->bAlwaysRelevant = true;
	Activated = false;
}

void ACooldownTimer::Initialize(const float CooldownUnits, ACannonCharacter* theOwner)
{
	Cooldown = CooldownUnits;
	TimeRemaining = CooldownUnits;
	Timer = 0.0f;
	SetCooldownTimerOwner(theOwner);
}

// Reduces Cooldown Timer by percent 
void ACooldownTimer::ReduceCooldown(float percent)
{
	if (!IsInactive()) // If the cooldown is active
	{
		float reducedTime = Cooldown * (percent / 100.0);

		Timer = Timer - reducedTime;
		TimeRemaining = TimeRemaining - reducedTime;
	}
}

// Calculate the next time the cooldown will be inactive
void ACooldownTimer::CalculateTimer()
{
	Timer = Cooldown + GetWorld()->GameState->GetServerWorldTimeSeconds();
	TimeRemaining = Cooldown;
}

// Returns the remaining time of the Cooldown
float ACooldownTimer::GetDisplayedTimer()
{
	float timeRemaining;

	IsInactive() ? timeRemaining = 0.0 : timeRemaining = FMath::RoundToInt(Timer - GetWorld()->GameState->GetServerWorldTimeSeconds());

	return timeRemaining;
}

// Returns if the cooldown is inactive
bool ACooldownTimer::IsInactive()
{
	return Timer < GetWorld()->GameState->GetServerWorldTimeSeconds();
}

bool ACooldownTimer::MulticastCooldownActive_Validate()
{
	return true;
}

void ACooldownTimer::MulticastCooldownActive_Implementation()
{
	Activated = true;
}

void ACooldownTimer::SetCooldownTimerOwner_Implementation(ACannonCharacter* theOwner)
{
	CooldownTimerOwner = theOwner;
}

void ACooldownTimer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACooldownTimer, Timer);
	DOREPLIFETIME(ACooldownTimer, TimeRemaining);
	DOREPLIFETIME(ACooldownTimer, Cooldown);
	DOREPLIFETIME(ACooldownTimer, Activated);
	DOREPLIFETIME(ACooldownTimer, CooldownTimerOwner);
}
