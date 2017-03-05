// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "PigProjectile.generated.h"

/**
 * 
 */
UCLASS()
class INCOWMING_API APigProjectile : public AProjectile
{
	GENERATED_BODY()
	

public:
	

	//Used to modify the initial speed of the pig. (power * initSpeedAlteration)
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounces)
		int nbOfBouncesRemaining;

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|PigProjectile")
		void OnProjectileBounce();

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|PigProjectile")
		void OnPigDestroy();

	APigProjectile();

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	void InitVelocity(const FVector& ShootDirection, const float Speed);
};
