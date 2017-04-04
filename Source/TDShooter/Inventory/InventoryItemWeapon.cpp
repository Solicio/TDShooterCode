// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "InventoryItemWeapon.h"




void AInventoryItemWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInventoryItemWeapon, StoredWeapon);
}