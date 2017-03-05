// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "DebugSettings.generated.h"

/**
 * 
 */
UCLASS()
class INCOWMING_API UDebugSettings : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UDebugSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Gravity;
	
	
};
