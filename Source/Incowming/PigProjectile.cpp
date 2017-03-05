// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "PigProjectile.h"



APigProjectile::APigProjectile()
{
	
	ProjectileMovement->ProjectileGravityScale = 3.0f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Friction = 3.1f;
	ProjectileMovement->Bounciness = 2.0f;
	nbOfBouncesRemaining = 3;
}

void APigProjectile::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("PIG PROJECTILE"));
}

void APigProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APigProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
	//if it hits another player
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		OnPigDestroy();
		Destroy();
	}
	//if last bounce
	else if (nbOfBouncesRemaining<1 )
	
	{
		
		OnPigDestroy();
		Destroy();
		
	}
	//it is a normal bounce
	else
	{
		OnProjectileBounce();
		nbOfBouncesRemaining--;
	}
}



void APigProjectile::InitVelocity(const FVector& ShootDirection, const float Speed)
{
	float newSpeed = initSpeedAlteration * Speed;

	Super::InitVelocity(ShootDirection, newSpeed);
}

