// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/Inventory/InventoryWidget.h"

#include "HUD/ShooterWidgetBase.h"
#include "StashWidget.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UStashWidget : public UShooterWidgetBase
{
	GENERATED_BODY()

	public:

		/** The players stash */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stash")
		UInventoryWidget* InventoryWidget;
	
		/** Updates the stash, refreshes inventory widget */
		UFUNCTION(BlueprintCallable, Category = "Stash")
		void UpdateStash();
	
};
