// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Damageable.generated.h"

/**
* This interface allows any class which inherits from it to implement
* a way to take damage.  
* 
* For instance, a projectile could check if the actor it hit was damageable;
* if so, apply some damage to it using the interface.
*/
UINTERFACE(Blueprintable)
class UDamageable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class INCOWMING_API IDamageable
{
	GENERATED_IINTERFACE_BODY()

public:
	// Classes using this interface must implement this with ReceiveDamage_Implementation(...).  See CannonCharacter for an example.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Incowming|Damageable")
	void ReceiveDamage(int32 Damage);
};