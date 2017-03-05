// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "Alien_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class INCOWMING_API AAlien_Projectile : public AProjectile
{
	GENERATED_BODY()
	
private:
	// Absolute Surface Tilt checks when the projectile hits an object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AbsSurfaceTilt;

	// Transform of the failure
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTransform FailureTransform;

public:
	// Sets default values for this actor's properties
	AAlien_Projectile();

	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Called every frame
	void Tick(float DeltaSeconds) override;

	// Called when CollisionComp hits any actor
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	// Called when CollisionComp hits any actor
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Initialises Velocity of the Projectile  
	//UFUNCTION(NetMulticast, Reliable)
	void InitVelocity(const FVector& ShootDirection, const float Speed) override;
	
	// Returns true if the zone is teleportable (Actually based on surface tilt but it might change soon)
	UFUNCTION(BlueprintCallable, Category = "Incowming|Alien_Projectile")
	bool IsTeleportable(FVector HitImpact);

	// Called when the teleportation failed
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Incowming|Alien_Projectile")
		void OnTeleportationFailed();

};
