// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "InventoryWidget.h"

void UInventoryWidget::UpdateInventory(TArray<AInventoryItemBase*> InventoryItemArray, AShooterCharacterBase* Owner)
{
	if (!Owner)
		return;

	if (GetPlayerHUDWidget() && GetPlayerHUDWidget()->GetShooterCharacter() && (InventoryItemArray.Num() == InventoryItemWidgets.Num()))
	{
		// Update the inventory widget array with the matching item in the inventory array
		for (int i = 0; i < InventoryItemWidgets.Num(); i++)
		{
			if (InventoryItemWidgets[i] != nullptr)
			{
				InventoryItemWidgets[i]->UpdateItem(InventoryItemArray[i], Owner, InventoryType);
			}
		}
	}
	// Array length mismatch or invalid pointers
	else
	{
		//Reset all the items
		for (auto Iter(InventoryItemWidgets.CreateIterator()); Iter; Iter++)
		{
			if ((*Iter) != nullptr)
				(*Iter)->ResetItem(Owner);
		}
	}
}

