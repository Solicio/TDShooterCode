// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryItemWidget.h"

#include "HUD/ShooterWidgetBase.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UInventoryWidget : public UShooterWidgetBase
{
	GENERATED_BODY()
	
	public:

		/** What type of widget this is. Is it the players inventory, a shop, or a stash */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Widget")
		EInventorySlotType InventoryType;
		
		/** All the inventory widget slots */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Widget")
		TArray<UInventoryItemWidget*> InventoryItemWidgets;


		/** Updates the inventory widget, including all inventory widgets 
		*	Sets all widget to match the provided inventory array.
		*/
		UFUNCTION(BlueprintCallable, Category = "Inventory Widget")
		void UpdateInventory(TArray<AInventoryItemBase*> InventoryItemArray, AShooterCharacterBase* Owner);

	protected:


};
