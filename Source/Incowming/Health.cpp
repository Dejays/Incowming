// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "Health.h"
#include "Net/UnrealNetwork.h"

AHealth::AHealth() 
{
	this->bReplicates = true;
	this->bAlwaysRelevant = true;
	Initialize(10, 10);
}

void AHealth::GainHealthUnits(const int32 Units)
{
	SetHealth(HealthUnits + Units);
}

void AHealth::LoseHealthUnits(const int32 Units)
{
	SetHealth(HealthUnits - Units);
}

void AHealth::Initialize(const int32 InitialHealth, const int32 MaximumHealthUnits)
{
	MaximumHealth = MaximumHealthUnits;
	SetHealth(InitialHealth);
}

void AHealth::SetHealth(const int32 Units)
{
	// Keep health unit in range 0 <= health <= MaximumHealth.
	HealthUnits = FMath::Min(FMath::Max(0, Units), MaximumHealth);
}

int32 AHealth::GetHealthUnits()
{
	return HealthUnits;
}

int32 AHealth::GetMaximumHealth()
{
	return MaximumHealth;
}

void AHealth::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHealth, HealthUnits);
	DOREPLIFETIME(AHealth, MaximumHealth);
}