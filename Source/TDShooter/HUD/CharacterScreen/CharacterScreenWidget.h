// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "HUD/Inventory/InventoryItemWidget.h"
#include "HUD/Inventory/InventoryWidget.h"

#include "HUD/ShooterWidgetBase.h"
#include "CharacterScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UCharacterScreenWidget : public UShooterWidgetBase
{
	GENERATED_BODY()
	
	public:

		/** The players inventory */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		UInventoryWidget* InventoryWidget;

		/** All the inventory slots */
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		//TArray<UInventoryItemWidget*> InventoryWidgets;

		/** Inventory widget for the main equipped weapon */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		UInventoryItemWidget* EquippedWeaponWidget;

		/** Inventory wudget for the secondaty equipped weapon */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		UInventoryItemWidget* SecEquippedWeaponWidget;

		/** Updates the character screen, refreshes inventory widgets and equiped items */
		UFUNCTION(BlueprintCallable, Category = "Character Screen")
		void UpdateScreen();


		///** Called from an inventory item widget when it gets pressed. */
		//UFUNCTION(BlueprintCallable, Category = "Character Screen")
		//void ItemPressed(UInventoryItemWidget* PressedItem);

		///** Called from an inventory item widget when it gets released. */
		//UFUNCTION(BlueprintCallable, Category = "Character Screen")
		//void ItemReleased();

		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		//UInventoryItemWidget* GrabbedItem;
};
