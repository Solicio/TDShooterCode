// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "IA_Container_Weapon.h"

#include "Characters/Player/ShooterCharacter.h"


AIA_Container_Weapon::AIA_Container_Weapon()
{
	LootType = ELootTypes::LT_Weapon;
	InteractRange = 300.0f;
}

void AIA_Container_Weapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIA_Container_Weapon, StoredWeapon);
}

void AIA_Container_Weapon::HandleContainer(AShooterCharacterBase* InteractingCharacter)
{
	if (InteractingCharacter && Role == ROLE_Authority && InteractingCharacter->ActorHasTag("Player"))
	{
		AShooterCharacter* Player = Cast<AShooterCharacter>(InteractingCharacter);
		
		// Check if the player has space in the inventory
		if (Player && Player->HasSpaceInInventory())
		{
			// Add the weapon into the inventory
			Player->AddInvItem(StoredWeapon, 0);
			
			// Then destroy itself
			Destroy();
		}
	}
}

void AIA_Container_Weapon::SetStoredWeapon(FStoredWeapon Weapon)
{
	if(Role == ROLE_Authority)
		StoredWeapon = Weapon;
}