// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "InputCannon.generated.h"

UCLASS()
class INCOWMING_API AInputCannon : public APawn
{
	GENERATED_BODY()

private:

	bool bPressed;        //Is the trigger pressed?
	float power,          //Canon launch power
		pressedTime;    //Time at which the trigger is pressed (for the Sine function)
	int selectedWeapon;   //Selected weapon (from 0 to 2 for now)

public:
	// Sets default values for this pawn's properties
	AInputCannon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere)
		USceneComponent* OurVisibleComponent;

	//Called when the trigger is pressed
	void UpdatePowerOn();

	//Called when the trigger is released
	void UpdatePowerOff();

	//Functions for the left and right shoulders to update the selected weapon
	void NextWeapon();
	void PreviousWeapon();

	//Accessor for the selected weapon
	int GetSelectedWeapon();
	
};
