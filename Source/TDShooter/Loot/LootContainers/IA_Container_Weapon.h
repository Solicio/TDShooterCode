// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Equipment/Weapons/BaseWeapon.h"

#include "Loot/IA_LootContainerBase.h"
#include "IA_Container_Weapon.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AIA_Container_Weapon : public AIA_LootContainerBase
{
	GENERATED_BODY()

	public:

		AIA_Container_Weapon();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon Container")
		FStoredWeapon StoredWeapon;

		/** Tell the container to handle it's logic.
		*	Weapon loot containers should add itself to the players inventory
		*	Health packs should increase the players health.
		*	Needs to be overriden in child classes.
		*/
		virtual void HandleContainer(AShooterCharacterBase* InteractingCharacter) override;
	
		/** Set the weapon that this container holds */
		UFUNCTION(BlueprintCallable, Category = "WeaponContainer")
		void SetStoredWeapon(FStoredWeapon Weapon);

};
