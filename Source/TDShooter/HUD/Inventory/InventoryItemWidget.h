// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/InventoryItemBase.h"
#include "Characters/ShooterCharacterBase.h"
#include "Global/RarityColorData.h"
#include "Global/ShooterStaticLibrary.h"

#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

class UShooterWidgetBase;

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()
	

	public:

		/** The item this widget will represent in the inventory */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		AInventoryItemBase* InventoryItem;
	
		/** Collection of all the different colors used for the different damage types */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		URarityColorData* ColorData;

		/** What type of widget this is. Is it the players inventory, an equipped item, etc.. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EInventorySlotType ItemWidgetType;

		/** What type of equipment this widget represents */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EItemTypes ItemWidgetEquipmentType;

		/** The character that owns this inventory */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		AShooterCharacterBase* OwningCharacter;


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FLinearColor DefaultBackgroundColor;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FLinearColor BackgroundColor;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FName ItemName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UTexture2D* ItemIcon;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool IsSlotHovered;

		/** The index the widget has in either an array or equipped items */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int32 WidgetIndex;


		/** Reference to the active tooltip widget */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UUserWidget* SpecialTooltipWidget;


		/** Parent widget of this widget */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UShooterWidgetBase* ParentWidget;
	

		/** Updates variables to show in the item slot, send in nullptr to reset the slot */
		UFUNCTION(BlueprintCallable, Category = "Item")
		void UpdateItem(AInventoryItemBase* Item, AShooterCharacterBase* Owner, EInventorySlotType SlotType);
	
		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Item")
		void BPUpdateItem();

		UFUNCTION(BlueprintCallable, Category = "Item")
		void ResetItem(AShooterCharacterBase* Owner);
		

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool IsGrabbed;

		/** Called from BP when the item has been released.
		*	Functions looks through all Inventory Item Widgets to see if any are hovered.
		*	Then contacts the player's character with what it should try and do.
		*/
		UFUNCTION(BlueprintCallable, Category = "Item")
		void ItemReleased();

		/** Called from BP when the grabbed item should be dropped on the floor */
		UFUNCTION(BlueprintCallable, Category = "Item")
		void DropItem();
};
