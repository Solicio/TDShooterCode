// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Equipment/Weapons/BaseWeapon.h"

#include "Inventory/InventoryItemBase.h"
#include "InventoryItemWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AInventoryItemWeapon : public AInventoryItemBase
{
	GENERATED_BODY()
	
	public:

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Item")
		FStoredWeapon StoredWeapon;
	
	
};
