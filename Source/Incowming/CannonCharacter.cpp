// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "CannonCharacter.h"
#include "Health.h"
#include "IncowmingGameState.h"
#include "IHeadMountedDisplay.h"
#include "IOculusLibraryPlugin.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "CooldownSystem.h"


// Sets default values
ACannonCharacter::ACannonCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Platform = CreateDefaultSubobject<USceneComponent>(TEXT("Platform"));
	Platform->SetupAttachment(RootComponent);
	Platform->SetIsReplicated(true);

	Barrel = CreateDefaultSubobject<USceneComponent>(TEXT("Barrel"));
	// Assign to the Platform component so any reset calls to the Barrel can reset to 0,0,0 relative to this component
	Barrel->SetupAttachment(Platform);
	Barrel->SetIsReplicated(true);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(Barrel);
	MuzzleLocation->SetIsReplicated(true);

	VROriginComp = CreateDefaultSubobject<USceneComponent>(TEXT("VRCameraOrigin"));
	VROriginComp->SetupAttachment(Platform);
	VROriginComp->SetIsReplicated(true);

	UIs = CreateDefaultSubobject<USceneComponent>(TEXT("UIs"));
	UIs->SetupAttachment(Platform);
	UIs->SetIsReplicated(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	// Assign to the VR origin component so any reset calls to the HMD can reset to 0,0,0 relative to this component
	CameraComp->SetupAttachment(VROriginComp);
	CameraComp->SetIsReplicated(true);

	PowerUI = CreateDefaultSubobject<USceneComponent>(TEXT("PowerUI"));
	PowerUI->SetupAttachment(UIs);
	PowerUI->SetIsReplicated(true);

	HealthUI = CreateDefaultSubobject<USceneComponent>(TEXT("HealthUI"));
	HealthUI->SetupAttachment(UIs);
	HealthUI->SetIsReplicated(true);

	PlayerNameComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("APlayerName"));
	PlayerNameComponent->SetupAttachment(UIs);
	PlayerNameComponent->SetIsReplicated(true);
	PlayerNameComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	PlayerNameComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	PlayerNameComponent->RelativeLocation = FVector(0.0f, 0.0f, 350.0f);
	PlayerNameComponent->WorldSize = 64.0f;
	PlayerNameComponent->SetTextRenderColor(FColor::Red);

	PlayerMessageComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("APlayerMessage"));
	PlayerMessageComp->SetupAttachment(UIs);
	PlayerMessageComp->SetOnlyOwnerSee(true);
	PlayerMessageComp->HorizontalAlignment = EHorizTextAligment::EHTA_Left;
	PlayerMessageComp->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	PlayerMessageComp->RelativeLocation = FVector(250.0f, -200.0f, 180.0f);
	PlayerMessageComp->RelativeRotation = FRotator(0.0f, 180.0f, 0.0f);
	PlayerMessageComp->WorldSize = 26.0f;
}

// Called when the game starts or when spawned
void ACannonCharacter::BeginPlay()
{
	UWorld* const World = GetWorld();
	GameState = World->GetGameState<AIncowmingGameState>();

	Health = World->SpawnActor<AHealth>();

	CooldownSystem = World->SpawnActor<ACooldownSystem>();

	PowerPointer = PowerUI->GetChildComponent(0)->GetChildComponent(0);
	HealthPointer = HealthUI->GetChildComponent(0)->GetChildComponent(0);

	SelectedWeapon = 0;
	InvulnerabilityTime = 0;

	Super::BeginPlay();

	SetupVROptions();

}

void ACannonCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACannonCharacter, Health);
	DOREPLIFETIME(ACannonCharacter, CooldownSystem);
	DOREPLIFETIME(ACannonCharacter, bTestingTeleportation);
	DOREPLIFETIME(ACannonCharacter, PlayerColor);
	DOREPLIFETIME(ACannonCharacter, bReady);
	DOREPLIFETIME(ACannonCharacter, bDead);
}

// Called every frame
void ACannonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (gameStarted && bPressed) {
		Power = (0.5f * FMath::Sin(PressedTime - 1.6f) + 1.0f) * 3000;    // ~ 0.5 Sin(0.1245x -1.6) + 1.0
		PressedTime += DeltaTime * 1.5f;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(Power));

		//FRotator Rotator = FRotator::ZeroRotator;

		//Rotator.Pitch += (-105 - (350 * Power) / 3000);

		//PowerPointer->SetRelativeRotation(Rotator);
	}
	/*else if (PowerPointer->GetComponentRotation().Pitch < 75 || PowerPointer->GetComponentRotation().Pitch > 79) {
		FRotator Rotator = FRotator::ZeroRotator;

		Power -= 50;
		Rotator.Pitch += (100 - (350 * Power) / 3000);

		PowerPointer->AddLocalRotation(FRotator(3, 0, 0));
		//PowerPointer->SetRelativeRotation(Rotator);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(PowerPointer->GetComponentRotation().Pitch));*/

	if (InvulnerabilityTime > 0)
		InvulnerabilityTime -= DeltaTime;
}

void ACannonCharacter::SetupVROptions()
{
	GetWorld()->Exec(GetWorld(), TEXT("stat fps"));

	IHeadMountedDisplay* HMD = (IHeadMountedDisplay*)(GEngine->HMDDevice.Get());

	if (HMD) HMD->EnableHMD(true);
	if (HMD && HMD->IsStereoEnabled())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("HMD connected/unavailable"));
		/* Disable/Enable positional movement to pin camera translation */
		HMD->EnablePositionalTracking(false);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HMD non connected/unavailable"));

}

void ACannonCharacter::ResetHMDOrigin()
{
	IHeadMountedDisplay* HMD = (IHeadMountedDisplay*)(GEngine->HMDDevice.Get());
	if (HMD && HMD->IsStereoEnabled())
	{
		HMD->ResetOrientation(0.0);
	}
}

// Called to bind functionality to input
void ACannonCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// set up gameplay key bindings
	InputComponent->BindAxis("TurnUpBarrel", this, &ACannonCharacter::TurnUpBarrel);
	//InputComponent->BindAxis("TurnRightBarrel", this, &ACannonCharacter::TurnRightBarrel);

	InputComponent->BindAxis("TurnPlatform", this, &ACannonCharacter::TurnRightPlatform);
	//InputComponent->BindAxis("LookUp", this, &ACannonCharacter::AddControllerPitchInput);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ACannonCharacter::StartLaunchPowerUpdate);
	InputComponent->BindAction("Fire", IE_Released, this, &ACannonCharacter::OnFire);

	InputComponent->BindAction("ChangeWeaponRight", IE_Pressed, this, &ACannonCharacter::NextWeapon);
	InputComponent->BindAction("ChangeWeaponLeft", IE_Pressed, this, &ACannonCharacter::PreviousWeapon);

	InputComponent->BindAction("ResetHMDOrigin", IE_Pressed, this, &ACannonCharacter::ResetHMDOrigin);

	InputComponent->BindAction("Select", IE_Pressed, this, &ACannonCharacter::SelectPressed);
	InputComponent->BindAction("StartGame", IE_Pressed, this, &ACannonCharacter::StartGame);
}


void ACannonCharacter::TurnUpBarrel(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		Value *= (GetWorld()->GetDeltaSeconds() * VERTICAL_ROTATE_SPEED);
		RotateBarrel(FRotator(Value, 0, 0), true);
	}
}

void ACannonCharacter::TurnRightBarrel(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		Value *= (GetWorld()->GetDeltaSeconds() * HORIZONTAL_ROTATE_SPEED);
		RotateBarrel(FRotator(0, Value, 0), false);
	}
}

void ACannonCharacter::TurnRightPlatform(float Value)
{
	Value *= (GetWorld()->GetDeltaSeconds() * HORIZONTAL_ROTATE_SPEED);
	if ((Controller != NULL) && (Value != 0.0f))
	{
		RotatePlatform(FRotator(0, Value, 0));
	}
}

void ACannonCharacter::SelectPressed() {
	if (GameState != NULL && GameState->GetCurrentMatchState() == EMatchStateEnum::WAITING) {
		SetReady(!bReady);
	}
}


void ACannonCharacter::RotatePlatform(FRotator Rotator) {
	// On effectue d'abord la rotation au niveau local, pour éviter le lag.
	Platform->AddLocalRotation(Rotator);
	// Puis on fait la rotation côté serveur, qui sera retransmise au joueur et corrigée si nécessaire.
	ServerRotatePlatform(Rotator);
}

void ACannonCharacter::ServerRotatePlatform_Implementation(FRotator Rotator) {
	Platform->AddLocalRotation(Rotator);
}

bool ACannonCharacter::ServerRotatePlatform_Validate(FRotator Rotator) { return true; }


void ACannonCharacter::RotateBarrel(FRotator Rotator, bool localRotation) {
	OnRotateBarrel();
	ServerRotateBarrel(Rotator, localRotation);
}

void ACannonCharacter::ServerRotateBarrel_Implementation(FRotator Rotator, bool localRotation)
{
	// TODO: Possiblement s'assurer que tout ça ne se calcule pas sur le client...?
	FRotator RelativeRotation = FRotator::MakeFromEuler(Barrel->GetRelativeTransform().GetRotation().Euler());

	// S'assurer que le canon sera dans un angle valide après la rotation
	float futurePitch = FRotator::NormalizeAxis(RelativeRotation.Pitch + Rotator.Pitch);
	float futureYaw = FRotator::NormalizeAxis(RelativeRotation.Yaw + Rotator.Yaw);

	// On effectue d'abord la rotation au niveau local, pour éviter le lag.
	// Puis on fait la rotation côté serveur, qui sera retransmise au joueur et corrigée si nécessaire.
	if ((futurePitch > -30 && futurePitch < 60) && localRotation) {
		Barrel->AddLocalRotation(Rotator);
	}

	if ((futureYaw > -60 && futureYaw < 60) && !localRotation) {
		Barrel->AddWorldRotation(Rotator);
	}
}

bool ACannonCharacter::ServerRotateBarrel_Validate(FRotator Rotator, bool localRotation) { return true; }


void ACannonCharacter::OnFire()
{
	// Check if the game is started (NOT FINISHED)
	if (GameState != NULL && GameState->GetCurrentMatchState() == EMatchStateEnum::WAITING)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't fire: match not started"));
		return;
	}

	bPressed = false;

	ServerFire(SelectedWeapon, this->Power, ProjectileClass);
	this->OnProjectileShot(ProjectileClass);

}

void ACannonCharacter::ServerFire_Implementation(int WeaponSelected, float PowerValue, TSubclassOf<class AProjectile> ProjectileType)
{
	if (CooldownSystem->IsProjectileInCooldown(WeaponSelected))
	{
		ClientProjectileLaunch(false);
		return;
	}
	// Find projectile class
	if (ProjectileType != NULL) {
		const FVector SpawnPoint = MuzzleLocation->GetComponentLocation();
		const FRotator BarrelRotation = FRotator::MakeFromEuler(Barrel->GetRelativeTransform().GetRotation().Euler());
		const FRotator PlatformRotation = FRotator::MakeFromEuler(Platform->GetRelativeTransform().GetRotation().Euler());
		const FRotator CannonRotation = FRotator::MakeFromEuler(this->GetRootComponent()->GetRelativeTransform().GetRotation().Euler());
		const FRotator SpawnRotation = BarrelRotation + (PlatformRotation + CannonRotation);

		UWorld* const World = GetWorld();

		if (World) {
			// Spawn projectile parameters
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileType, SpawnPoint, SpawnRotation, SpawnParams);
			if (Projectile)
			{
				FVector const LaunchDir = SpawnRotation.Vector();
				Projectile->SetProjectileOwner(this);
				Projectile->InitVelocity(LaunchDir, PowerValue);
				ClientProjectileLaunch(true);
			}	
		}
	}
}

bool ACannonCharacter::ServerFire_Validate(int WeaponSelected, float PowerValue, TSubclassOf<class AProjectile> ProjectileType) { return true; }



void ACannonCharacter::ClientProjectileLaunch_Implementation(bool Launched)
{
	if (Launched)
		OnProjectileLaunch();
}

bool ACannonCharacter::ClientProjectileLaunch_Validate(bool Launched) { return true; }

void ACannonCharacter::StartLaunchPowerUpdate() {
	Power = 0;
	PressedTime = 0;
	bPressed = true;
}


void ACannonCharacter::NextWeapon() {
	SelectedWeapon >= 2 ? SelectedWeapon = 0 : SelectedWeapon++;
	UpdateWeapon();
	this->OnWeaponChange();
}

void ACannonCharacter::PreviousWeapon() {
	SelectedWeapon <= 0 ? SelectedWeapon = 2 : SelectedWeapon--;
	UpdateWeapon();
	this->OnWeaponChange();
}

void ACannonCharacter::UpdateWeapon() {
	switch (SelectedWeapon) {
	case 0:
		ProjectileClass = LoadClass <AProjectile>(NULL, TEXT("Blueprint'/Game/Blueprints/BPCowProjectile.BPCowProjectile_C'"), NULL, LOAD_None, NULL);
		break;
	case 1:
		ProjectileClass = LoadClass <AProjectile>(NULL, TEXT("Blueprint'/Game/Blueprints/BPPigProjectile.BPPigProjectile_C'"), NULL, LOAD_None, NULL);
		break;
	case 2:
		ProjectileClass = LoadClass <AProjectile>(NULL, TEXT("Blueprint'/Game/Blueprints/BP_Alien_Projectile.BP_Alien_Projectile_C'"), NULL, LOAD_None, NULL);
		break;
	}
}

int ACannonCharacter::GetSelectedWeapon() {
	return SelectedWeapon;
}

void ACannonCharacter::UpdateHealthPointer(int32 NewHealth)
{
	HealthPointer->AddLocalRotation(FRotator(-24, 0, 0));
}

// Appelé par serveur
void ACannonCharacter::SetPlayerColor(FLinearColor color)
{
	PlayerColor = color;
}

// Gameloop Start functions
void ACannonCharacter::SetReady_Implementation(bool Ready)
{
	bReady = Ready;
	GameState->UpdateNbPlayersReady();
}
bool ACannonCharacter::SetReady_Validate(bool Ready) { return true; }

void ACannonCharacter::StartGame_Implementation()
{
	if (GameState->GetCurrentMatchState() != EMatchStateEnum::WAITING ||
		GameState->GetNbPlayers() != GameState->GetNbPlayersReady())
	{
		return;
	}

	GameState->StartGame(EMatchStateEnum::STARTED);
}
bool ACannonCharacter::StartGame_Validate() { return true; }

void ACannonCharacter::HandleGameStarted_Implementation()
{
	gameStarted = true;
	ToggleStartGameWidget(false);
} 
bool ACannonCharacter::HandleGameStarted_Validate() { return true; }

// Client Function OnDamageEnemy
void ACannonCharacter::ServerEnemyDamaged(ACannonCharacter* Enemy)
{
	ClientEnemyDamaged();
}

bool ACannonCharacter::ClientEnemyDamaged_Validate() { return true; }

void ACannonCharacter::ClientEnemyDamaged_Implementation()
{
	OnEnemyDamaged();
}

// Client Function OnWin
bool ACannonCharacter::ClientYouWin_Validate() { return true; }

void ACannonCharacter::ClientYouWin_Implementation()
{
	OnYouWin();
}

// Client Function OnReceiveDamage
void ACannonCharacter::ReceiveDamage_Implementation(int32 Damage) {
	if (Damage <= 0 || InvulnerabilityTime > 0)
		return;

	InvulnerabilityTime = 2.0;
	//UE_LOG(LogTemp, Warning, TEXT("Server - Hit player had %d health."), Health->GetHealthUnits());
	Health->LoseHealthUnits(Damage);
	//UE_LOG(LogTemp, Warning, TEXT("Server - Hit player now has %d health."), Health->GetHealthUnits());

	OnDamageReceivedServer();

	if (Health->GetHealthUnits() <= 0)
	{
		bDead = true;
		GameState->UpdateNbPlayersDead(FName(*PlayerState->PlayerName));
	}
	else
		ClientDamageReceived(Health->GetHealthUnits());
}

bool ACannonCharacter::ClientDamageReceived_Validate(const int32 NewHealth) { return true; }

void ACannonCharacter::ClientDamageReceived_Implementation(const int32 NewHealth)
{
	// Blueprint implementation
	OnDamageReceived(NewHealth);
}

// Client Function OnLose
bool ACannonCharacter::ClientYouLose_Validate() { return true; }

void ACannonCharacter::ClientYouLose_Implementation()
{
	OnYouLose();
}

// Client Function MessageReceived
void ACannonCharacter::ClientMessageReceived_Implementation(FName message)
{
	OnMessageReceived(message.ToString());
}

bool ACannonCharacter::ClientMessageReceived_Validate(FName message) { return true; }

// Server Function TeleportationSuccessed
void ACannonCharacter::ServerTeleportationSuccessed(FVector futurePosition, const FRotator futureRotation)
{
	ClientTeleportationSuccessed();
	OnTeleportation(futurePosition, futureRotation);
}

void ACannonCharacter::ClientTeleportationSuccessed_Implementation()
{
	OnTeleportationSuccessed();
}

bool ACannonCharacter::ClientTeleportationSuccessed_Validate() { return true; }

// Server Function TeleportationFailed
void ACannonCharacter::ServerTeleportationFailed(FTransform failureTransform)
{
	ClientTeleportationFailed(failureTransform);
}

// Client Function TeleportationFailed
void ACannonCharacter::ClientTeleportationFailed_Implementation(FTransform failLocation)
{
	OnTeleportationFailed(failLocation);
}

bool ACannonCharacter::ClientTeleportationFailed_Validate(FTransform failLocation) { return true; }