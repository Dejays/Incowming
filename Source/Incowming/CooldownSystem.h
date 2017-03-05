// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CowProjectile.h"
#include "PigProjectile.h"
#include "Alien_Projectile.h"
#include "CooldownTimer.h"
#include "CooldownSystem.generated.h"


UCLASS()
class INCOWMING_API ACooldownSystem : public AActor
{
	GENERATED_BODY()

public:	
	ACooldownSystem();

	// Spawns CooldownTimers
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownSystem")
	void Initialize(const float CowCooldown, const float PigCooldown, const float AlienCooldown, ACannonCharacter* theOwner);

	// Returns if the ProjectileType cooldows is active and set it if not (executed from server) 
	UFUNCTION(BlueprintCallable, Category = "Incowming|CooldownSystem")
	bool IsProjectileInCooldown(int SelectedWeapon);

	void ACooldownSystem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class ACooldownTimer* AlienCooldown;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class ACooldownTimer* PigCooldown;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class ACooldownTimer* CowCooldown;
	
};
