// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "StashWidget.h"




void UStashWidget::UpdateStash()
{

	if (!GetPlayerHUDWidget() || !GetPlayerHUDWidget()->GetShooterCharacter())
		return;

	// Update the inventory
	if (InventoryWidget)
	{
		InventoryWidget->UpdateInventory(GetPlayerHUDWidget()->GetShooterCharacter()->StashInventory, GetPlayerHUDWidget()->GetShooterCharacter());
	}
}