// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "Projectile.h"
#include "CannonCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Damageable.h"
#include "DebugSettings.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetupDebugOptions();
	
}

void AProjectile::SetupDebugOptions()
{
	UDebugSettings* DebugSettings = Cast<UDebugSettings>(GetGameInstance());
	if (DebugSettings)
	{
		float desiredGravity = DebugSettings->Gravity;
		if (desiredGravity != NULL)
		{
			SetProjectileGravity(desiredGravity);
		}
	}
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Changer la gravité du projectile sur le serveur
void AProjectile::SetProjectileGravity_Implementation(float Gravity)
{
	ProjectileMovement->ProjectileGravityScale = Gravity;
	PropagateProjectileGravity(Gravity);
}

bool AProjectile::SetProjectileGravity_Validate(float Gravity)
{
	return true;
}

// Envoyer la nouvelle gravité aux clients
void AProjectile::PropagateProjectileGravity_Implementation(float Gravity)
{
	ProjectileMovement->ProjectileGravityScale = Gravity;
}

// Cette fonction est multicast: elle est exécutée sur le serveur puis sur les clients
void AProjectile::InitVelocity_Implementation(const FVector& ShootDirection, const float Speed)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * Speed;
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Role == ROLE_Authority)
	{
		this->HandleHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	}
	else
	{
	}
}

void AProjectile::HandleHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TArray<AActor*> ActorsDamaged = ApplyDamage(OtherActor);
	if (ActorsDamaged.Num() == 0)
	{
		// Projectile didn't deal any damage.
		return;
	}


	// Projectile dealt damage.
	// If player who launched it dealt damage to something other
	// than himself, notify him his shot was successful.
	if (projectileOwner)
	{
		for (AActor* Actor : ActorsDamaged)
		{
			if (Actor != nullptr && Actor->IsA(ACannonCharacter::StaticClass()))
			{
				ACannonCharacter* Enemy = Cast<ACannonCharacter>(Actor);
				if (Enemy && Enemy != projectileOwner)
				{
					projectileOwner->ServerEnemyDamaged(Enemy);
					break;
				}
			}
		}
	}
}

bool AProjectile::HandleHit_Validate(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	return true;
}

ACannonCharacter* AProjectile::GetProjectileOwner()
{
	return projectileOwner;
}

void AProjectile::SetProjectileOwner_Implementation(ACannonCharacter* theOwner)
{
	this->projectileOwner = theOwner;
}

TArray<AActor*> AProjectile::ApplyDamage(AActor* ActorHit)
{
	TArray<AActor*> ActorsDamaged;
	if (ActorHit && ActorHit->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		IDamageable* DamageableActor = Cast<IDamageable>(ActorHit);
		if (DamageableActor)
		{
			DamageableActor->ReceiveDamage_Implementation(Damage);
			ActorsDamaged.Push(ActorHit);
		}
	}

	return ActorsDamaged;
}
