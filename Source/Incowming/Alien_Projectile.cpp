// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "Alien_Projectile.h"
#include "CannonCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAlien_Projectile::AAlien_Projectile()
{
	ProjectileMovement->ProjectileGravityScale = 10.0f;
}

// Called when the game starts or when spawned
void AAlien_Projectile::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ALIEN_PROJECTILE"));
}

// Called every frame
void AAlien_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAlien_Projectile::InitVelocity(const FVector& ShootDirection, const float Speed)
{
	//if (ProjectileMovement)
	//{
		float newSpeed = initSpeedAlteration * Speed;

		Super::InitVelocity(ShootDirection, newSpeed);
		//ProjectileMovement->Velocity = ShootDirection * Speed * 2;
	//}
}

void AAlien_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Role == ROLE_Authority)
	{
		this->HandleHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	}
}

void AAlien_Projectile::HandleHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (projectileOwner) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, projectileOwner->GetName());
		TSubclassOf<ACannonCharacter> CannonClass = LoadClass <ACannonCharacter>(NULL, TEXT("Blueprint'/Game/Blueprints/BP_CannonCharacter.BP_CannonCharacter_C'"), NULL, LOAD_None, NULL);
		if (IsTeleportable(Hit.ImpactNormal) && OtherActor->GetClass() != CannonClass)
		{
			FVector TeleportPosition = Hit.ImpactPoint + FVector(0.0f, 0.0f, 64.0f);
			FRotator CannonRotation = FRotator::MakeFromEuler(projectileOwner->GetRootComponent()->GetRelativeTransform().GetRotation().Euler());

			projectileOwner->ServerTeleportationSuccessed(TeleportPosition, CannonRotation);
		}
		else
		{
			OnTeleportationFailed();
			projectileOwner->ServerTeleportationFailed(FailureTransform);
		}

		Destroy();
	}
}

bool AAlien_Projectile::HandleHit_Validate(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	return true;
}

bool AAlien_Projectile::IsTeleportable(FVector HitImpact) 
{
	float surfaceTilt = UKismetMathLibrary::MakeRotFromY(HitImpact).Pitch;
	// Actually based on surface tilt but it might change soon
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Surface Tilt : %f"), surfaceTilt));

	return (surfaceTilt >= -AbsSurfaceTilt && surfaceTilt <= AbsSurfaceTilt);
}
