// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "TestProjectile.h"


// Sets default values
ATestProjectile::ATestProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision compoennt
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	RootComponent = CollisionComp;

	// Projectile movement component
	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void ATestProjectile::BeginPlay()
{
	Super::BeginPlay();

	DespawnTime = 3;

}

// Called every frame
void ATestProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DespawnTime -= DeltaTime;
	
	if (DespawnTime < 0)
		this->Destroy();

}

void ATestProjectile::InitVelocity(const FVector& ShootDirection, const float Speed)
{
	if (ProjectileMovement)
	{
		//ProjectileMovement->Velocity = ShootDirection * Speed;
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}