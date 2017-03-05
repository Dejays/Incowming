// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Projectile.h"
#include "CowProjectile.h"
#include "PigProjectile.h"
#include "Alien_Projectile.h"
#include "Damageable.h"

#include "CannonCharacter.generated.h"

UCLASS()
class INCOWMING_API ACannonCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	/********************
	* PUBLIC PROPERTIES *
	*********************/
	/*************** Actors ***************/
	// Health
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Incowming|CannonCharacter")
		class AHealth* Health;

	// Cooldown system
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Incowming|CannonCharacter", meta = (AllowPrivateAccess = "true"))
		class ACooldownSystem* CooldownSystem;

	/************** Objects **************/
	// Projectile Class
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> ProjectileClass;

	// Unique color for each player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		FLinearColor PlayerColor;

	/************** Values **************/
	//Is teleportation effect activated?
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int bTestingTeleportation;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		bool bReady = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		bool bDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool gameStarted;

	static const int HORIZONTAL_ROTATE_SPEED = 40;
	static const int VERTICAL_ROTATE_SPEED = 70;

	/********************
	 * PUBLIC FUNCTIONS *
	 *******************/
	// Sets default values for this character's properties
	ACannonCharacter();

	/*********** UE4 Events/Functions ***********/
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	void ACannonCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	/************ Input Functions ************/
	UFUNCTION() //handles Pitch angle of cannon
		void TurnUpBarrel(float Value);
	UFUNCTION() //handles Yaw angle of cannon
		void TurnRightBarrel(float Value);
	UFUNCTION() //handles Yaw angle of platform
		void TurnRightPlatform(float Value);

	/********** Triggered by Input **********/
	// Rotate barrel
	virtual void RotateBarrel(FRotator Rotator, bool localRotation);	// Client
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRotateBarrel(FRotator Rotator, bool localRotation);	// Server

	// Fire a projectile
	void OnFire();																		// Client
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(int WeaponSelected, float PowerValue, TSubclassOf<class AProjectile> ProjectileType);	// Server

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
	void ClientProjectileLaunch(bool Launched);

	//Event when firing a projectile
	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
	void OnProjectileLaunch();

	// Rotate the platform
	virtual void RotatePlatform(FRotator Rotator);	// Client
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRotatePlatform(FRotator Rotator);	// Server

	//Functions for the left and right shoulders to update the selected weapon
	void NextWeapon();
	void PreviousWeapon();

	/*********** Blueprint Events ***********/
	// Called when the barrel is moved
	UFUNCTION(BlueprintImplementableEvent)
		void OnRotateBarrel();

	//called when a player presses "PreviousWeapon" or "NextWeapon"
	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponChange();

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnProjectileShot(TSubclassOf<class AProjectile> ProjectileType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnDamageReceived(const int32 NewHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnDamageReceivedServer();

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnEnemyDamaged();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Incowming|CannonCharacter")
		void OnTeleportation(FVector futurePosition, const FRotator futureRotation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnTeleportationFailed(FTransform failLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnTeleportationSuccessed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void ToggleStartGameWidget(bool visible);

	/*************** Getters ***************/
	//Accessor for the selected weapon
	UFUNCTION(BlueprintCallable, Category = "Incowming|CannonCharacter")
		int GetSelectedWeapon();

	/*********** Other UFunctions ***********/
	// Resets HMD Origin position and orientation
	UFUNCTION(BlueprintCallable, Category = "Incowming|CannonCharacter")
		void ResetHMDOrigin();

	UFUNCTION(Client, Reliable, WithValidation)
		void ClientDamageReceived(const int32 NewHealth);

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
		void ClientEnemyDamaged();

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
		void ClientTeleportationFailed(FTransform failLocation);

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
		void ClientTeleportationSuccessed();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingCharacter|GameStart")
		void SetReady(bool Ready);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "IncowmingCharacter|GameStart")
		void StartGame();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void HandleGameStarted();

	// Updates the health arrow rotation on the cannon.
	UFUNCTION(BlueprintCallable, Category = "Incowming|CannonCharacter")
		void UpdateHealthPointer(int32 NewHealth);

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
		void ClientYouWin();
	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnYouWin();

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
		void ClientYouLose();
	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnYouLose();

	UFUNCTION(Client, Reliable, WithValidation, Category = "Incowming|CannonCharacter")
		void ClientMessageReceived(FName message);
	UFUNCTION(BlueprintImplementableEvent, Category = "Incowming|CannonCharacter")
		void OnMessageReceived(const FString& message);

	/************ Other Functions ************/
	void SetPlayerColor(FLinearColor color);
	void StartLaunchPowerUpdate();
	void UpdatePower(float value);
	void SetupVROptions();
	void ReceiveDamage_Implementation(int32 Damage); // Called by server
	void ServerEnemyDamaged(ACannonCharacter* Enemy);  // Called by server
	UFUNCTION(BlueprintCallable, Category = "Incowming|CannonCharacter")
	void ServerTeleportationFailed(FTransform failureTransform);  // Called by server
	UFUNCTION(BlueprintCallable, Category = "Incowming|CannonCharacter")
	void ServerTeleportationSuccessed(FVector futurePosition, const FRotator futureRotation);  // Called by server
	void SelectPressed();

private:
	/*********************
	* PRIVATE PROPERTIES *
	**********************/
	/************** Values **************/
	//Is the trigger pressed?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPressed; 

	//Canon launch power
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Power;

	//Time at which the trigger is pressed (for the Sine function)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float PressedTime;    

	float InvulnerabilityTime;

	//Selected weapon (from 0 to 2 for now)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int SelectedWeapon;  
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AIncowmingGameState* GameState;

	/************ Components ************/
	// Component to specify origin for the HMD
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* VROriginComp;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	// Uis component contains 3D UMG elements
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* UIs;

	// Barrel component constains cannon elements
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Barrel;

	// Platform component contains platform elements
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Platform;

	// Location on cannon mesh where projectiles should spawn.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USceneComponent* MuzzleLocation;

	// Power indicator component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* PowerPointer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* PowerUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HealthPointer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HealthUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* PlayerNameComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* PlayerMessageComp;
	/********************
	* PRIVATE FUNCTIONS *
	********************/
	void UpdateWeapon();
};
