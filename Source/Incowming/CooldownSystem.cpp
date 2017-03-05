#include "Incowming.h"
#include "CooldownSystem.h"
#include "Net/UnrealNetwork.h"
#include "CannonCharacter.h"

ACooldownSystem::ACooldownSystem()
{
	this->bReplicates = true;
	this->bAlwaysRelevant = true;
}

// Spawns CooldownTimers
void ACooldownSystem::Initialize(const float CowCooldown, const float PigCooldown, const float AlienCooldown, ACannonCharacter* theOwner)
{
	UWorld* const World = GetWorld();

	this->AlienCooldown->Initialize(AlienCooldown, theOwner);

	this->PigCooldown->Initialize(PigCooldown, theOwner);

	this->CowCooldown->Initialize(CowCooldown, theOwner);
}

// Returns if the ProjectileType cooldows is active and set it if not (executed from server) 
bool ACooldownSystem::IsProjectileInCooldown(int SelectedWeapon)
{
	switch (SelectedWeapon)
	{
	case 0:
		if (CowCooldown && CowCooldown->IsInactive()) // If inactive calculate the next time they will be inactive
		{
			CowCooldown->CalculateTimer();
			CowCooldown->MulticastCooldownActive();
			return false; // the projectile is not in cooldown
		}
		break;
	case 1:
		if (PigCooldown && PigCooldown->IsInactive()) // If inactive calculate the next time they will be inactive
		{
			PigCooldown->CalculateTimer();
			PigCooldown->MulticastCooldownActive();
			return false; // the projectile is not in cooldown
		}
		break;
	case 2:
		if (AlienCooldown && AlienCooldown->IsInactive()) // If inactive calculate the next time they will be inactive
		{
			AlienCooldown->CalculateTimer();
			AlienCooldown->MulticastCooldownActive();
			return false; // the projectile is not in cooldown
		}
		break;
	}
	
	return true; // the projectile is in cooldown
}

void ACooldownSystem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACooldownSystem, PigCooldown);
	DOREPLIFETIME(ACooldownSystem, AlienCooldown);
	DOREPLIFETIME(ACooldownSystem, CowCooldown);
}