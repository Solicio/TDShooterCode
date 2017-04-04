// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Loot/IA_LootContainerBase.h"
#include "IA_Container_HealthPack.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AIA_Container_HealthPack : public AIA_LootContainerBase
{
	GENERATED_BODY()
	
	public:

		AIA_Container_HealthPack();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Pack")
		float HealthIncrease;
	
		/** Tell the container to handle it's logic.
		*	Weapon loot containers should add itself to the players inventory
		*	Health packs should increase the players health.
		*	Needs to be overriden in child classes.
		*/
		virtual void HandleContainer(AShooterCharacterBase* InteractingCharacter) override;

};
