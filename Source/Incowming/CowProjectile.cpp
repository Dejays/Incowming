// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "CowProjectile.h"
#include "Damageable.h"
#include "CannonCharacter.h"

ACowProjectile::ACowProjectile()
{
	
	ProjectileMovement->ProjectileGravityScale = 5.0f;
}

void ACowProjectile::BeginPlay() 
{
	Super::BeginPlay();
	//ProjectileMovement->InitialSpeed = ProjectileMovement->InitialSpeed * 0.01f;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("COW PROJECTILE"));
}

void ACowProjectile::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
}

void ACowProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	Destroy();
}


void ACowProjectile::InitVelocity(const FVector& ShootDirection, const float Speed) 
{
	float newSpeed = initSpeedAlteration * Speed;
	
	Super::InitVelocity(ShootDirection, newSpeed);
}

TArray<AActor*> ACowProjectile::ApplyDamage(AActor* ActorHit)
{
	TArray<AActor*> ActorsDamaged;

	// This projectile deals <Damage> for a direct hit, and
	// <ExplosionDamage> for every damageable inside its explosion
	// radius.

	IDamageable* DamageableHit = nullptr;
	if (ActorHit && ActorHit->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		DamageableHit = Cast<IDamageable>(ActorHit);
	}

	if (ExplosionRadiusReference)
	{
		TArray<AActor*> Actors;
		ExplosionRadiusReference->GetOverlappingActors(Actors);
		for (AActor* Actor : Actors)
		{
			if (Actor && Actor->GetClass()->ImplementsInterface(UDamageable::StaticClass()) && Actor != projectileOwner)
			{
				IDamageable* DamageableOverlapped = Cast<IDamageable>(Actor);
				int DamageToDeal = Damage;
				if (DamageableHit)
				{
					if (Actor == ActorHit)
					{
						// Because a damageable hit will always be inside the explosion radius
						// (assuming the radius is always at least as big as the cow), we apply its
						// damage here to ensure direct hit damage and AoE damage happens at the same time.
						DamageToDeal += ExplosionDamage;
					}
				}

				DamageableOverlapped->ReceiveDamage_Implementation(DamageToDeal);
				ActorsDamaged.Push(Actor);
			}
			else
			{

			}
		}
	}
	else
	{

	}

	return ActorsDamaged;
}