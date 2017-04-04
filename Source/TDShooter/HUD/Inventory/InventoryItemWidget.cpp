// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "InventoryItemWidget.h"

#include "Inventory/InventoryItemWeapon.h"
#include "HUD/ShooterWidgetBase.h"

void UInventoryItemWidget::UpdateItem(AInventoryItemBase* Item, AShooterCharacterBase* Owner, EInventorySlotType SlotType)
{
	InventoryItem = Item;

	// Set the owner of this widget
	OwningCharacter = Owner;

	SpecialTooltipWidget = nullptr;
	IsGrabbed = false;
	IsSlotHovered = false;

	if (InventoryItem == nullptr)
	{
		ResetItem(Owner);

		return;
	}

	// Set the name
	ItemName = InventoryItem->ItemName;

	// Set the icon
	ItemIcon = InventoryItem->ItemIcon;
	
	// Set the type of equipment this widget represent
	ItemWidgetEquipmentType = InventoryItem->ItemType;

	// Make sure the widget and the item has the right type.
	ItemWidgetType = SlotType;
	InventoryItem->ItemSlotType = SlotType;

	if (InventoryItem->ItemType == EItemTypes::IT_Weapon && ColorData)
	{
		AInventoryItemWeapon* InvItemWeaopn;
		InvItemWeaopn = Cast<AInventoryItemWeapon>(InventoryItem);

		// set the background color to match the rarity
		if (InvItemWeaopn)
		{
			switch (InvItemWeaopn->StoredWeapon.Rarity)
			{
				case ERarity::R_Common:
					BackgroundColor = ColorData->Colors.Common;
					break;
				case ERarity::R_Uncommon:
					BackgroundColor = ColorData->Colors.Uncommon;
					break;
				case ERarity::R_Rare:
					BackgroundColor = ColorData->Colors.Rare;
					break;
				case ERarity::R_Epic:
					BackgroundColor = ColorData->Colors.Epic;
					break;
				case ERarity::R_Legendary:
					BackgroundColor = ColorData->Colors.Legendary;
					break;
			};
		}
	}

	BPUpdateItem();
}

void UInventoryItemWidget::ResetItem(AShooterCharacterBase* Owner)
{
	OwningCharacter = Owner;

	BackgroundColor = DefaultBackgroundColor;
	ItemName = " ";
	ItemIcon = nullptr;
	InventoryItem = nullptr;
	IsGrabbed = false;
	IsSlotHovered = false;
	SpecialTooltipWidget = nullptr;

	if(ItemWidgetType != EInventorySlotType::IST_Equipped)
		ItemWidgetEquipmentType = EItemTypes::IT_None;
}

void UInventoryItemWidget::ItemReleased()
{
	//if (!IsGrabbed || !InventoryItem)
	//	return;

	// First look through all the widgets to find the one that was grabbed and dropped on this widget.
	for (TObjectIterator<UInventoryItemWidget> Itr; Itr; ++Itr)
	{
		UInventoryItemWidget* GrabbedWidget = (*Itr);

		if (GrabbedWidget != nullptr && GrabbedWidget->IsValidLowLevel() && GrabbedWidget != this)
		{
			// Found a slot that is grabbed.
			if (GrabbedWidget->IsGrabbed)
			{
				// If the player is dragging this item onto a equipment slot, make sure they are matching types
				if (ItemWidgetType == EInventorySlotType::IST_Equipped &&
					(ItemWidgetEquipmentType == GrabbedWidget->ItemWidgetEquipmentType))
				{
					ParentWidget->GetPlayerHUDWidget()->GetShooterCharacter()->MoveInventoryItem(GrabbedWidget->InventoryItem, GrabbedWidget->OwningCharacter, WidgetIndex, ItemWidgetType, OwningCharacter);
				}
				// If the hovered widget is not a equipment slot
				else if (ItemWidgetType != EInventorySlotType::IST_Equipped)
				{
					ParentWidget->GetPlayerHUDWidget()->GetShooterCharacter()->MoveInventoryItem(GrabbedWidget->InventoryItem, GrabbedWidget->OwningCharacter, WidgetIndex, ItemWidgetType, OwningCharacter);
				}
			
				GrabbedWidget->IsGrabbed = false;
			}
		}
	}

	IsGrabbed = false;


	//// Look through all the inventory widget slots to see if any of them are hovered.
	//for (TObjectIterator<UInventoryItemWidget> Itr; Itr; ++Itr)
	//{
	//	UInventoryItemWidget* HoveredWidget = (*Itr);

	//	if (HoveredWidget != nullptr && HoveredWidget->IsValidLowLevel())
	//	{
	//		// Found a slot that is hovered.
	//		if (HoveredWidget != this && HoveredWidget->IsSlotHovered)
	//		{
	//			// If the player is dragging this item onto a equipment slot, make sure they are matching types
	//			if (HoveredWidget->ItemWidgetType == EInventorySlotType::IST_Equipped &&
	//				(ItemWidgetEquipmentType == HoveredWidget->ItemWidgetEquipmentType))
	//			{
	//				ParentWidget->GetPlayerHUDWidget()->GetShooterCharacter()->MoveInventoryItem(InventoryItem, OwningCharacter, HoveredWidget->WidgetIndex, HoveredWidget->ItemWidgetType, HoveredWidget->OwningCharacter);
	//			}
	//			// If the hovered widget is not a equipment slot
	//			else if (HoveredWidget->ItemWidgetType != EInventorySlotType::IST_Equipped)
	//			{
	//				ParentWidget->GetPlayerHUDWidget()->GetShooterCharacter()->MoveInventoryItem(InventoryItem, OwningCharacter, HoveredWidget->WidgetIndex, HoveredWidget->ItemWidgetType, HoveredWidget->OwningCharacter);
	//			}
	//		}
	//	}
	//}

	//IsGrabbed = false;
}

void UInventoryItemWidget::DropItem()
{
	PrintText("Drop Item!");
	IsGrabbed = false;
}