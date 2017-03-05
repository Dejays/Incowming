// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Health.generated.h"

/**
 * This class allows to handle any object's health.  You should first
 * initialize your parameters with Initialize method, then use it with
 * Gain and Lose methods.
 */
UCLASS()
class INCOWMING_API AHealth : public AActor
{
	GENERATED_BODY()

public:
	AHealth();

	UFUNCTION(BlueprintCallable, Category = "Incowming|Health")
	void GainHealthUnits(const int32 Units);

	UFUNCTION(BlueprintCallable, Category = "Incowming|Health")
	void LoseHealthUnits(const int32 Units);

	UFUNCTION(BlueprintCallable, Category = "Incowming|Health")
	void Initialize(const int32 InitialHealth, const int32 MaximumHealth);

	UFUNCTION(BlueprintCallable, Category = "Incowming|Health")
	int32 GetHealthUnits();

	UFUNCTION(BlueprintCallable, Category = "Incowming|Health")
	int32 GetMaximumHealth();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 HealthUnits;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaximumHealth = 100;

	void SetHealth(int32 Units);
};
