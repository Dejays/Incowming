// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"
#include "InputCannon.h"


// Sets default values
AInputCannon::AInputCannon() {
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// Create a visible object
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// Attach visible object to our root component.
	OurVisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInputCannon::BeginPlay() {
	Super::BeginPlay();
	
	selectedWeapon = 0;
}

// Called every frame
void AInputCannon::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if (bPressed) {
		power = 0.5f * FMath::Sin(pressedTime - 1.6f) + 0.5f;    // ~ 0.5 Sin(0.1245x -1.6) + 0.5
		pressedTime += DeltaTime * 1.5f;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(power));
	}
}

// Called to bind functionality to input
void AInputCannon::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("CannonPower", IE_Pressed, this, &AInputCannon::UpdatePowerOn);
	InputComponent->BindAction("CannonPower", IE_Released, this, &AInputCannon::UpdatePowerOff);

	InputComponent->BindAction("ChangeWeaponRight", IE_Pressed, this, &AInputCannon::NextWeapon);
	InputComponent->BindAction("ChangeWeaponLeft", IE_Pressed, this, &AInputCannon::PreviousWeapon);
}

void AInputCannon::UpdatePowerOn() {
	power = 0;
	pressedTime = 0;
	bPressed = true;
}

void AInputCannon::UpdatePowerOff() {
	bPressed = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(power));
}

void AInputCannon::NextWeapon() {
	selectedWeapon >= 2 ? selectedWeapon = 0 : selectedWeapon++;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::FromInt(selectedWeapon));
}

void AInputCannon::PreviousWeapon() {
	selectedWeapon <= 0 ? selectedWeapon = 2 : selectedWeapon--;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::FromInt(selectedWeapon));
}

int AInputCannon::GetSelectedWeapon() {
	return selectedWeapon;
}

