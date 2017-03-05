// Fill out your copyright notice in the Description page of Project Settings.

#include "Incowming.h"

#include "AutomationTest.h"
#include "Health.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHealthTest, "Incowming Health", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FHealthTest::RunTest(const FString& Parameters)
{
	// Given
	AHealth* Health = NewObject<AHealth>();
	TestNotNull(TEXT("Health instantiation failed."), Health);

	// Test parameters initialization
	Health->Initialize(50, 100);
	TestEqual(TEXT("Initialization failed (initial health value."), 50, Health->GetHealthUnits());
	TestEqual(TEXT("Initialization failed (maximum health value."), 100, Health->GetMaximumHealth());

	// Test basic gain/lose health
	Health->GainHealthUnits(1);
	TestEqual(TEXT("Gain health not working"), 51, Health->GetHealthUnits());

	Health->LoseHealthUnits(1);
	TestEqual(TEXT("Lose health not working."), 50, Health->GetHealthUnits());

	// Test minimum and maximum (range)
	Health->GainHealthUnits(200);
	TestEqual(TEXT("Health can't be higher than maximum allowed."), 100, Health->GetHealthUnits());

	Health->LoseHealthUnits(200);
	TestEqual(TEXT("Health can't be lower than 0."), 0, Health->GetHealthUnits());

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
