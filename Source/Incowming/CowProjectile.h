// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "CowProjectile.generated.h"

/**
 * 
 */
UCLASS()
class INCOWMING_API ACowProjectile : public AProjectile
{
	GENERATED_BODY()
	
public :
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Incowming|Projectile")
	UStaticMeshComponent* ExplosionRadiusReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Incowming|Projectile")
	int32 ExplosionDamage;

	ACowProjectile();

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	void InitVelocity(const FVector& ShootDirection, const float Speed);
		 
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	TArray<AActor*> ApplyDamage(AActor* ActorHit) override;
};
