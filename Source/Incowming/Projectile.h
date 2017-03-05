// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class ACannonCharacter;



UCLASS()
class INCOWMING_API AProjectile : public AActor
{
	GENERATED_BODY()

protected:
	// Sphere collision componen
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Incowming|Projectile")
		float initSpeedAlteration = 1.0f;

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	// Damage this projectile deals when it hits an object which inherits IDamageable.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Incowming|Projectile")
	int32 Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//The canonCharacter who fired the projectile. Michaël Déry: Note - Voir si on ne pourrait pas retirer ça et utiliser l'instigateur de l'acteur ou encore l'owner.
	ACannonCharacter *projectileOwner;
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	//Set the canonCharacter who fired the projectile
	UFUNCTION(NetMulticast, Reliable)
	void SetProjectileOwner(ACannonCharacter* theOwner);

	//Get canonCharacter who fired the projectile
	ACannonCharacter* GetProjectileOwner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupDebugOptions();
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Set projectile gravity on the server
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void SetProjectileGravity(float Gravity);

	// Update projectile gravity on clients
	UFUNCTION(NetMulticast, Reliable)
	virtual void PropagateProjectileGravity(float Gravity);

	// Called when CollisionComp hits any actor
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Initialises Velocity of the Projectile  
	UFUNCTION(NetMulticast, Reliable)
	virtual void InitVelocity(const FVector& ShootDirection, const float Speed);

	// Returns CollisionComp subobject
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	// Returns ProjectileMovement subobject 
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
protected:
	// Returns actors damaged.
	virtual TArray<class AActor*> ApplyDamage(AActor* ActorHit);
};
