// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "IA_Container_HealthPack.h"



AIA_Container_HealthPack::AIA_Container_HealthPack()
{
	HealthIncrease = 100.0f;

	LootType = ELootTypes::LT_HealthPack;
	InteractRange = 150.0f;
	RangeTriggeredInteraction = true;
	PlayerOnlyTrigger = true;
}

void AIA_Container_HealthPack::HandleContainer(AShooterCharacterBase* InteractingCharacter)
{
	if (InteractingCharacter && Role == ROLE_Authority)
	{
		// Add the health increase to the players HP
		InteractingCharacter->HealthCurrent = FMath::Clamp(InteractingCharacter->HealthCurrent + HealthIncrease, 0.0f, InteractingCharacter->HealthMax);

		// Then destroy itself
		Destroy();
	}
}