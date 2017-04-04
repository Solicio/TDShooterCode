// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "ShooterCharacter.h"

#include "ShooterPC.h"
#include "HUD/PlayerHUDWidget.h"
#include "Inventory/InventoryItemWeapon.h"
#include "Characters/NPC/NPC_Base.h"
#include "General/TDShooterGameState.h"


AShooterCharacter::AShooterCharacter()
{
	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Height of characters jump
	RunningJumpZVelocity = 500.0f;
	StandingJumpZVelocity = 620.0f;

	// Store the walkspeed for later use.
	NormalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	SprintMultiplier = 2.0f;

	// Configure character movement
	GetCharacterMovement()->AirControl = 0.2f;

	InventorySize = 24;

	Tags.Add("Player");
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set the inventory size
	Inventory.SetNum(InventorySize);
	StashInventory.SetNum(InventorySize);
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, Inventory);
	DOREPLIFETIME(AShooterCharacter, StashInventory);
	DOREPLIFETIME(AShooterCharacter, EquippedWeaponInvItem);
	DOREPLIFETIME(AShooterCharacter, SecEquippedWeaponInvItem);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AShooterCharacter::HasSpaceInInventory()
{
	// Loop through all the elements and check if there is a nullptr
	for (auto Iter(Inventory.CreateIterator()); Iter; Iter++)
	{
		// If there is a nullptr, there is an empty slot
		if ((*Iter) == nullptr)
		{
			return true;
		}
	}

	return false;
}

void AShooterCharacter::AddInvItem(FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory)
{
	// If the client is making the call, make sure the server knows
	if (Role < ROLE_Authority)
	{
		ServerAddInvItem(StoredWeapon, Index, FirstEmpty, IntoInventory);
		
		return;
	}

	AddInvItemWeapon(StoredWeapon, Index, FirstEmpty, IntoInventory);

	UpdateCharacterScreen();
	UpdateStashWidget();
}

void AShooterCharacter::ServerAddInvItem_Implementation(FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory)
{
	// Call the function again, but on the server
	AddInvItem(StoredWeapon, Index, FirstEmpty, IntoInventory);
}

bool AShooterCharacter::ServerAddInvItem_Validate(FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory)
{
	return true;
}


void AShooterCharacter::AddInvItemWeapon(FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory)
{
	// Make sure it's added on the server
	if (Role == ROLE_Authority)
	{
		// Array to add item into, depending on inpur parameter
		TArray<AInventoryItemBase*>& ArrayToUse = (IntoInventory) ? Inventory : StashInventory;

		// Look for an empty slot, only spawn the actor if there is an empty slot
		int32 ArrayIndex = 0;
		bool FoundEmpty = false;

		// If the provided index should be used, check if the slot is empty
		if (!FirstEmpty)
		{
			ArrayIndex = Index;
			FoundEmpty = (ArrayToUse[ArrayIndex] == nullptr);
		}
		
		// If first empty should be used or if the provided index was not empty
		if(FirstEmpty || !FoundEmpty)
		{
			FoundEmpty = ArrayToUse.Find(nullptr, ArrayIndex);
		}

		
		if (FoundEmpty)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Spawn the inventory item
			AInventoryItemWeapon* SpawnedItem;
			SpawnedItem = GetWorld()->SpawnActor<AInventoryItemWeapon>(AInventoryItemWeapon::StaticClass(), FTransform::Identity, SpawnInfo);

			// Return if spawn failed
			if (!SpawnedItem)
				return;

			// Set the values on the item
			SpawnedItem->StoredWeapon = StoredWeapon;
			SpawnedItem->ItemName = StoredWeapon.EquipmentName;
			SpawnedItem->ItemIcon = SpawnedItem->StoredWeapon.EquipmentIcon;
			SpawnedItem->ItemIndex = ArrayIndex;
			SpawnedItem->ItemType = EItemTypes::IT_Weapon;
			SpawnedItem->ItemSlotType = (IntoInventory) ? EInventorySlotType::IST_Inventory : EInventorySlotType::IST_Stash;

			// Add the item to the empty slot
			ArrayToUse[ArrayIndex] = SpawnedItem;

			//PrintText("Added Item! Index: " + FString::FromInt(Index), -1, 2.0f, FColor::Green);
		}
	}
}

//void AShooterCharacter::RemoveInventoryItem(int32 Index)
//{
//	// Make sure it's removed on the server
//	if (Role == ROLE_Authority)
//	{
//		Inventory[Index]->Destroy();
//		Inventory[Index] = nullptr;
//	}
//}

void AShooterCharacter::MoveInventoryItem(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter, 
										  int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter)
{

	// Call this function on the server to have the slots switch place
	if (Role < ROLE_Authority)
	{
		ServerMoveInventoryItem(GrabbedItem, SendingCharacter, TargetIndex, TargetType, ReceivingCharacter);
		UpdateCharacterScreen();
		UpdateStashWidget();
		return;
	}

	// Stop if the pointers are invalid
	if (!SendingCharacter || !ReceivingCharacter || !GrabbedItem)
		return;


	// The index from which the item was moved, either from array or from equipped item slot
	int32 SentFromIndex = GrabbedItem->ItemIndex;

	// Pointer to the Receiving and sending arrays
	TArray<AInventoryItemBase*>* SendingArrayPointer = nullptr;
	TArray<AInventoryItemBase*>* ReceivingArrayPointer = nullptr;

	// Used if the player wants to equip and item.
	bool WantsToEquip = false;
	bool WantsToDeEquip = false;
	
	bool SenderIsInventory = true;
	bool TargetIsInventory = true;

	// First get where the item was sent from, was it an inventory or was it equipped, etc.
	switch (GrabbedItem->ItemSlotType)
	{
		// Was sent from the inventory array
		case EInventorySlotType::IST_Inventory:
			SendingArrayPointer = &Inventory;
			SenderIsInventory = true;
			//PrintText("Grabbed Item : From Inventory", -1, 10.0f);
			break;

		// Was sent from the stash array
		case EInventorySlotType::IST_Stash:
			SendingArrayPointer = &StashInventory;
			SenderIsInventory = false;
			//PrintText("Grabbed Item : From Stash", -1, 10.0f);
			break;

		// Shop should be handled differently
		case EInventorySlotType::IST_Shop:
			return;
			break;

		case EInventorySlotType::IST_Equipped:
			WantsToDeEquip = true;
			break;
	}

	// Then get what should be receiving the item
	switch (TargetType)
	{
		case EInventorySlotType::IST_Inventory:
			ReceivingArrayPointer = &Inventory;
			TargetIsInventory = true;
			//PrintText("Target Type : Inventory", -1, 10.0f);
			break;

		case EInventorySlotType::IST_Stash:
			ReceivingArrayPointer = &StashInventory;
			TargetIsInventory = false;
			//PrintText("Target Type : Stash", -1, 10.0f);
			break;

		// Shop should be handled differently
		case EInventorySlotType::IST_Shop:
			return;
			break;

		case EInventorySlotType::IST_Equipped:
			WantsToEquip = true;
			break;
	}

	// If the movement is between inventory or stash
	if (SendingArrayPointer && ReceivingArrayPointer)
	{
		// Get the arrays from the pointers
		TArray<AInventoryItemBase*>& SendingArray = *SendingArrayPointer;
		TArray<AInventoryItemBase*>& ReceivingArray = *ReceivingArrayPointer;

		// Store the value we are overwriting in case it needs to be moved to the sending location
		AInventoryItemBase* ItemBackup = ReceivingArray[TargetIndex];

		// Set the value and index 
		ReceivingArray[TargetIndex] = GrabbedItem;
		ReceivingArray[TargetIndex]->ItemIndex = TargetIndex;

		// Remove the old reference 
		SendingArray[SentFromIndex] = nullptr;

		// If there was an item in the receiving slot, move that value to the original location
		if (ItemBackup != nullptr)
		{
			// Set the value and index 
			SendingArray[SentFromIndex] = ItemBackup;
			SendingArray[SentFromIndex]->ItemIndex = SentFromIndex;
		}
	}
	// If the movement is between inventory/stash and equipped item
	else if (SendingArrayPointer && WantsToEquip)
	{
		TArray<AInventoryItemBase*>& SendingArray = *SendingArrayPointer;

		// Store the value we are overwriting in case it needs to be moved to the sending location
		bool Success = false;
		FStoredWeapon StoredBackup;

		// If the player wants to equip a weapon
		if (GrabbedItem->ItemType == EItemTypes::IT_Weapon)
		{
			// Cast to the weapon class to get access to the stored weapon struct
			AInventoryItemWeapon* GrabbedWeapon = Cast<AInventoryItemWeapon>(GrabbedItem);

			// If the player dragged the item onto the first weapon slot
			if (TargetIndex == 0)
			{
				// Remove the previous weapon
				if (EquippedWeapon)
				{
					StoredBackup = EquippedWeapon->GetStoredWeapon(Success);
					EquippedWeapon->Destroy();
				}

				EquippedWeapon = nullptr;

				// Create and equip the weapon
				if (GetWorld()->GetGameState() && Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager())
					EquipWeapon(Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager()->CreateStoredWeapon(GrabbedWeapon->StoredWeapon), 0);

			}
			// If the player dragged the item onto the second weapon slot
			else if (TargetIndex == 1)
			{
				// Remove the previous weapon
				if (SecondaryEquippedWeapon)
				{
					StoredBackup = SecondaryEquippedWeapon->GetStoredWeapon(Success);
					SecondaryEquippedWeapon->Destroy();
				}

				SecondaryEquippedWeapon = nullptr;

				// Create and equip the weapon
				if (GetWorld()->GetGameState() && Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager())
					EquipWeapon(Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager()->CreateStoredWeapon(GrabbedWeapon->StoredWeapon), 1);

			}
		}

		// Remove the old item if there is one
		if (SendingArray[SentFromIndex])
			SendingArray[SentFromIndex]->Destroy();

		SendingArray[SentFromIndex] = nullptr;
		
		// Add a new inventory item based on the previous weapon, if there was one
		if(Success)
			AddInvItemWeapon(StoredBackup, SentFromIndex, false, SenderIsInventory);
	}
	else if (WantsToDeEquip && ReceivingArrayPointer)
	{
		// Get the array from the pointers
		TArray<AInventoryItemBase*>& ReceivingArray = *ReceivingArrayPointer;
		
		// Store the value we are overwriting in case it needs to be moved to the sending location
		AInventoryItemBase* ItemBackup = ReceivingArray[TargetIndex];

		// Set the element to nullptr so that a new item can be added.
		ReceivingArray[TargetIndex] = nullptr;

		// Store the value we are overwriting in case it needs to be moved to the sending location
		bool Success = false;
		FStoredWeapon StoredBackup;

		// If the player wants to de-equip a weapon
		if (GrabbedItem->ItemType == EItemTypes::IT_Weapon)
		{
			// Player dragging from the first slot
			if (GrabbedItem->ItemIndex == 0)
			{
				// Create a new inventory item based on the current weapon
				if (EquippedWeapon)
				{
					AddInvItemWeapon(EquippedWeapon->GetStoredWeapon(Success), TargetIndex, false, TargetIsInventory);
					EquippedWeapon->Destroy();
					EquippedWeapon = nullptr;
				}

				if (ItemBackup)
				{
					// Cast to the weapon class to get access to the stored weapon struct
					StoredBackup = Cast<AInventoryItemWeapon>(ItemBackup)->StoredWeapon;

					// Create and equip the weapon
					if (GetWorld()->GetGameState() && Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager())
						EquipWeapon(Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager()->CreateStoredWeapon(StoredBackup), 0);

					// Remove the backup
					ItemBackup->Destroy();
					ItemBackup = nullptr;
				}
			}
			// Player dragging from the second slot
			else if (GrabbedItem->ItemIndex == 1)
			{
				// Create a new inventory item based on the current weapon
				if (SecondaryEquippedWeapon)
				{
					AddInvItemWeapon(SecondaryEquippedWeapon->GetStoredWeapon(Success), TargetIndex, false, TargetIsInventory);
					SecondaryEquippedWeapon->Destroy();
					SecondaryEquippedWeapon = nullptr;
				}

				if (ItemBackup)
				{
					// Cast to the weapon class to get access to the stored weapon struct
					AInventoryItemWeapon* ItemBackupWeapon = Cast<AInventoryItemWeapon>(ItemBackup);
					StoredBackup = ItemBackupWeapon->StoredWeapon;

					// Create and equip the weapon
					if (GetWorld()->GetGameState() && Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager())
						EquipWeapon(Cast<ATDShooterGameState>(GetWorld()->GetGameState())->GetEquipmentManager()->CreateStoredWeapon(StoredBackup), 1);

					// Remove the backup
					ItemBackup->Destroy();
					ItemBackup = nullptr;
				}
			}

			// Update the inventory items that represents the weapons
			UpdateEquippedInvWeapons(false);
		}
	}
	// If the player wants to switch places of equipped items. Just switch places.
	else if (WantsToDeEquip && WantsToEquip)
	{
		SwitchEquippedWeapons();
	}

	UpdateCharacterScreen();
	UpdateStashWidget();
}

void AShooterCharacter::ServerMoveInventoryItem_Implementation(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter,
															   int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter)
{
	MoveInventoryItem(GrabbedItem, SendingCharacter, TargetIndex, TargetType, ReceivingCharacter);
}

bool AShooterCharacter::ServerMoveInventoryItem_Validate(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter,
														 int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter)
{
	return true;
}

void AShooterCharacter::DropInventoryItem(int32 GrabbedSlot)
{

}

void AShooterCharacter::EquipInventoryItem(int32 GrabbedSlot)
{

}


void AShooterCharacter::InteractWith(AInteractableActor* InteractActor)
{
	if (Role == ROLE_Authority)
	{
		if (InteractActor)
		{
			// Check the distance between the character and interact object
			float Distance = FVector::Dist(InteractActor->GetActorLocation(), GetActorLocation());

			// Check again if it can be used, but this time on the server
			if (Distance < InteractActor->InteractRange && !InteractActor->RangeTriggeredInteraction && InteractActor->Active && !InteractActor->InteractedWith)
			{
				// Call the interactable actor that it should be interacted with
				InteractActor->Interact(this);
			}
		}
	}
	else
	{
		if (InteractActor)
		{
			// On client, make the call on server
			ServerInteractWith(InteractActor);
		}
	}
}

void AShooterCharacter::ServerInteractWith_Implementation(AInteractableActor* InteractActor)
{
	InteractWith(InteractActor);
}

bool AShooterCharacter::ServerInteractWith_Validate(AInteractableActor* InteractActor)
{
	return true;
}


void AShooterCharacter::SetMuzzleLookAt(FVector LookAtPoint)
{
	if (Role == ROLE_Authority)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->SetMuzzleLookAt(LookAtPoint);
		}
	}
	else
	{
		// On client, call the server
		ServerSetMuzzleLookAt(LookAtPoint);
	}
}

void AShooterCharacter::ServerSetMuzzleLookAt_Implementation(FVector LookAtPoint)
{
	SetMuzzleLookAt(LookAtPoint);
}

bool AShooterCharacter::ServerSetMuzzleLookAt_Validate(FVector LookAtPoint)
{
	return true;
}



void AShooterCharacter::MakeNewCombatText(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit)
{
	// Since this is on the HUD, make Sure it's only called on the local character
	ClientMakeNewCombatText(CombatText, Location, DamageType, IsCrit);
}

void AShooterCharacter::ClientMakeNewCombatText_Implementation(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit)
{
	// Tell the HUD to show the combat text.
	AShooterPC* ShooterController = Cast<AShooterPC>(GetController());
	if (ShooterController && ShooterController->PlayerHUDWidget)
	{
		ShooterController->PlayerHUDWidget->NewCombatText(CombatText, Location, DamageType, IsCrit);
	}
}

bool AShooterCharacter::ClientMakeNewCombatText_Validate(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit)
{
	return true;
}

void AShooterCharacter::UpdateCharacterScreen()
{
	// Update character screen and stash widget on the hud
	AShooterPC* ShooterController = Cast<AShooterPC>(GetController());
	if (ShooterController && ShooterController->PlayerHUDWidget)
	{
		ShooterController->PlayerHUDWidget->UpdateCharacterScreen();
	}
}

void AShooterCharacter::UpdateStashWidget()
{
	// Update character screen and stash widget on the hud
	AShooterPC* ShooterController = Cast<AShooterPC>(GetController());
	if (ShooterController && ShooterController->PlayerHUDWidget)
	{
		ShooterController->PlayerHUDWidget->UpdateStashWidget();
	}
}


void AShooterCharacter::RefreshEquippedInvWeapons()
{
	if (Role < ROLE_Authority)
		return;

	// Refresh the ammo count on the stored weapon structs in the inventory / equipment
	if (EquippedWeaponInvItem && EquippedWeapon)
	{
		Cast<AInventoryItemWeapon>(EquippedWeaponInvItem)->StoredWeapon.AmmoCountCurrent = EquippedWeapon->AmmoCountCurrent;
	}

	if (SecEquippedWeaponInvItem && SecondaryEquippedWeapon)
	{
		Cast<AInventoryItemWeapon>(SecEquippedWeaponInvItem)->StoredWeapon.AmmoCountCurrent = SecondaryEquippedWeapon->AmmoCountCurrent;
	}
}

void AShooterCharacter::UpdateEquippedInvWeapons(bool SwappedWeapons)
{
	if (Role < ROLE_Authority)
		return;

	// Both weapons are nullptrs, destroy the inventory items if there are any then stop
	if (!EquippedWeapon && !SecondaryEquippedWeapon)
	{
		if (EquippedWeaponInvItem)
			EquippedWeaponInvItem->Destroy();

		if (SecEquippedWeaponInvItem)
			SecEquippedWeaponInvItem->Destroy();
			
		SecEquippedWeaponInvItem = nullptr;
		EquippedWeaponInvItem = nullptr;

		UpdateCharacterScreen();
		return;
	}

	// If the player just swapped the weapons and there is one that is valid, just swap the references
	if (SwappedWeapons && (EquippedWeaponInvItem || SecEquippedWeaponInvItem))
	{
		AInventoryItemBase* TempInvItem = EquippedWeaponInvItem;

		EquippedWeaponInvItem = SecEquippedWeaponInvItem;
		SecEquippedWeaponInvItem = TempInvItem;

		if(EquippedWeaponInvItem)
			EquippedWeaponInvItem->ItemIndex = 0;

		if(SecEquippedWeaponInvItem)
			SecEquippedWeaponInvItem->ItemIndex = 1;

		UpdateCharacterScreen();
		return;
	}

	// If there is atleast one valid weapon and the player did not just swap the weapons.
	// Create new inventory items for the weapons.

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AInventoryItemWeapon* SpawnedItem;
	bool Success = false;
	
	if (EquippedWeapon)
	{
		// If there already is an item referenced, destroy it first
		if (EquippedWeaponInvItem)
		{
			EquippedWeaponInvItem->Destroy();
			EquippedWeaponInvItem = nullptr;
		}

		// Spawn for the main equipped weapon
		// Spawn the inventory item
		SpawnedItem = GetWorld()->SpawnActor<AInventoryItemWeapon>(AInventoryItemWeapon::StaticClass(), FTransform::Identity, SpawnInfo);

		// Return if spawn failed
		if (!SpawnedItem)
			return;

		// Set the values on the item
		SpawnedItem->StoredWeapon = EquippedWeapon->GetStoredWeapon(Success);
		SpawnedItem->ItemName = EquippedWeapon->EquipmentName;
		SpawnedItem->ItemIcon = SpawnedItem->StoredWeapon.EquipmentIcon;
		SpawnedItem->ItemType = EItemTypes::IT_Weapon;
		SpawnedItem->ItemSlotType = EInventorySlotType::IST_Equipped;
		SpawnedItem->ItemIndex = 0;
		
		// Set the reference
		EquippedWeaponInvItem = SpawnedItem;
	}
	// If there isn't a weapon but an inv item, destroy it
	else if (!EquippedWeapon && EquippedWeaponInvItem)
	{
		EquippedWeaponInvItem->Destroy();
		EquippedWeaponInvItem = nullptr;
	}

	if (SecondaryEquippedWeapon)
	{
		// If there already is an item referenced, destroy it first
		if (SecEquippedWeaponInvItem)
		{
			SecEquippedWeaponInvItem->Destroy();
			SecEquippedWeaponInvItem = nullptr;
		}

		// Spawn for the main equipped weapon
		// Spawn the inventory item
		SpawnedItem = GetWorld()->SpawnActor<AInventoryItemWeapon>(AInventoryItemWeapon::StaticClass(), FTransform::Identity, SpawnInfo);

		// Return if spawn failed
		if (!SpawnedItem)
			return;

		// Set the values on the item
		SpawnedItem->StoredWeapon = SecondaryEquippedWeapon->GetStoredWeapon(Success);
		SpawnedItem->ItemName = SecondaryEquippedWeapon->EquipmentName;
		SpawnedItem->ItemIcon = SpawnedItem->StoredWeapon.EquipmentIcon;
		SpawnedItem->ItemType = EItemTypes::IT_Weapon;
		SpawnedItem->ItemSlotType = EInventorySlotType::IST_Equipped;
		SpawnedItem->ItemIndex = 1;

		// Set the reference
		SecEquippedWeaponInvItem = SpawnedItem;
	}
	// If there isn't a weapon but an inv item, destroy it
	else if (!SecondaryEquippedWeapon && SecEquippedWeaponInvItem)
	{
		SecEquippedWeaponInvItem->Destroy();
		SecEquippedWeaponInvItem = nullptr;
	}

	UpdateCharacterScreen();
}
