// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "CharacterScreenWidget.h"

void UCharacterScreenWidget::UpdateScreen()
{
	//// Array length mismatch or invalid pointers
	//if (!GetPlayerHUDWidget() || !GetPlayerHUDWidget()->GetShooterCharacter() || (GetPlayerHUDWidget()->GetShooterCharacter()->Inventory.Num() != InventoryWidgets.Num()))
	//{
	//	//Reset all the items
	//	for (auto Iter(InventoryWidgets.CreateIterator()); Iter; Iter++)
	//	{
	//		if((*Iter) != nullptr)
	//			(*Iter)->ResetItem();
	//	}
	//}
	//else
	//{
	//	// Update the inventory widget array with the matching item in the inventory array
	//	for (int i = 0; i < InventoryWidgets.Num(); i++)
	//	{
	//		if (InventoryWidgets[i] != nullptr)
	//			InventoryWidgets[i]->UpdateItem(GetPlayerHUDWidget()->GetShooterCharacter()->Inventory[i]);
	//	}
	//}

	if (!GetPlayerHUDWidget() || !GetPlayerHUDWidget()->GetShooterCharacter())
		return;

	// Update the inventory
	if(InventoryWidget)
		InventoryWidget->UpdateInventory(GetPlayerHUDWidget()->GetShooterCharacter()->Inventory, GetPlayerHUDWidget()->GetShooterCharacter());

	// Update the equipped items
	if (EquippedWeaponWidget)
		EquippedWeaponWidget->UpdateItem(GetPlayerHUDWidget()->GetShooterCharacter()->EquippedWeaponInvItem, GetPlayerHUDWidget()->GetShooterCharacter(), EInventorySlotType::IST_Equipped);

	if (SecEquippedWeaponWidget)
		SecEquippedWeaponWidget->UpdateItem(GetPlayerHUDWidget()->GetShooterCharacter()->SecEquippedWeaponInvItem, GetPlayerHUDWidget()->GetShooterCharacter(), EInventorySlotType::IST_Equipped);

}

//void UCharacterScreenWidget::ItemPressed(UInventoryItemWidget* PressedItem)
//{
//	//if (PressedItem && PressedItem->InventoryItem)
//	//	GrabbedItem = PressedItem;
//}
//
//void UCharacterScreenWidget::ItemReleased()
//{
//	//if (!GrabbedItem && !GrabbedItem->InventoryItem)
//	//	return;
//
//	////CombinedArray.Add(EquippedWeaponWidget);
//	////CombinedArray.Add(SecEquippedWeaponWidget);
//
//	//// Moving items in the inventory
//	//for (int i = 0; i < InventoryWidgets.Num(); i++)
//	//{
//	//	if (InventoryWidgets[i] != nullptr && InventoryWidgets[i]->IsSlotHovered)
//	//	{
//	//		PrintText(InventoryWidgets[i]->GetName(), -1, 2.0f, FColor::Green);
//	//		
//	//		// Move the elements
//	//		GetPlayerHUDWidget()->GetShooterCharacter()->MoveInventoryItem(GrabbedItem->InventoryItem->ArrayIndex, i);
//	//		GetPlayerHUDWidget()->UpdateCharacterScreen();
//	//		return;
//	//		
//	//	}
//	//}
//	//
//	//// Check if player wants to equip
//
//	//// If the character screen widget is hovered
//	//// Put back the item
//	//if (IsHovered())
//	//{
//	//	PrintText("Character Screen", -1, 2.0f, FColor::Green);
//	//	GrabbedItem = nullptr;
//	//	//GetPlayerHUDWidget()->UpdateCharacterScreen();
//	//	return;
//	//}
//
//	//// Nothing hovered
//	//// Drop item
//	//PrintText("Drop Item", -1, 2.0f, FColor::Green);
//	////GetPlayerHUDWidget()->UpdateCharacterScreen();
//	//GrabbedItem = nullptr;
//}