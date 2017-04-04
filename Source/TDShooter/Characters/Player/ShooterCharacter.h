// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/ShooterCharacterBase.h"

#include "Interaction/InteractableActor.h"

#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class TDSHOOTER_API AShooterCharacter : public AShooterCharacterBase
{
	GENERATED_BODY()

	public:
		
		AShooterCharacter();


		//--- Movement Variables ---//

		/** Height of the character jump when standing still */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float StandingJumpZVelocity;

		/** Height of the character jump when running */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float RunningJumpZVelocity;

		/** Stores the normal walkspeed of the character, taken from character movement comp */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float NormalWalkSpeed;

		/** Multiplier used to determine running speed when sprinting */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float SprintMultiplier;
		

		//--- Inventory & Stash ---//

		/** Array containing all the inventory items */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = UpdateCharacterScreen, Category = "Inventory & Stash")
		TArray<AInventoryItemBase*> Inventory;

		/** Array containing all the stash inventory items */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = UpdateStashWidget, Category = "Inventory & Stash")
		TArray<AInventoryItemBase*> StashInventory;

		/** Amount of items that can fit in the inventory */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory & Stash")
		int32 InventorySize;

		/** Checks if there is any empty spots in the inventory that can be filled */
		UFUNCTION(BlueprintCallable, Category = "Inventory & Stash")
		bool HasSpaceInInventory();


		/** Adds an item to the array. If call is on client, call to the server
		*	Adds an item to the array based on provided ItemContainer.
		*/
		UFUNCTION(BlueprintCallable, Category = "Inventory & Stash")
		void AddInvItem(/* ALootContainer LootContainer */FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty = true, bool IntoInventory = true);

		/** Adds a weapon to the inventory based on StoredWeapon. Only works on server*/
		UFUNCTION(BlueprintCallable, Category = "Inventory & Stash")
		void AddInvItemWeapon(FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory);

		/** Removes an item from the array. If call is on client, call to the server 
		*	Destroys element at index.
		*/
		//UFUNCTION(BlueprintCallable, Category = "Character Screen")
		//void RemoveInventoryItem(int32 Index);


		/** Player wants to move an inventory item to a different slot on the HUD */
		UFUNCTION(BlueprintCallable, Category = "Inventory & Stash")
		void MoveInventoryItem(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter, 
							   int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter);

		/** Player wants to drop the item on the floor */
		UFUNCTION(BlueprintCallable, Category = "Inventory & Stash")
		void DropInventoryItem(int32 GrabbedSlot);

		/** Player wants to equip the item */
		UFUNCTION(BlueprintCallable, Category = "Inventory & Stash")
		void EquipInventoryItem(int32 GrabbedSlot);


		//--- Character Screen ---//
		
		/** Inventory item that represents the equipped weapon */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = UpdateCharacterScreen, Category = "Character Screen")
		AInventoryItemBase* EquippedWeaponInvItem;

		/** Inventory item that represents the secondary equipped weapon */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = UpdateCharacterScreen, Category = "Character Screen")
		AInventoryItemBase* SecEquippedWeaponInvItem;


		//--- Interacting ---//

		/** If the player wants to interact with a actor.
		*	Tells the server that the player wants to interact with the actor.
		*/
		UFUNCTION(BlueprintCallable, Category = "Interacting")
		void InteractWith(AInteractableActor* InteractActor);


		//--- General Functions ---//

		/** Calls the HUD that it should show a new combat text element. */
		void MakeNewCombatText(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit);

		void SetMuzzleLookAt(FVector LookAtPoint);


	protected:

		UFUNCTION()
		void UpdateCharacterScreen();

		UFUNCTION()
		void UpdateStashWidget();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddInvItem(/* ALootContainer LootContainer */FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory);
		void ServerAddInvItem_Implementation(/* ALootContainer LootContainer */FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory);
		bool ServerAddInvItem_Validate(/* ALootContainer LootContainer */FStoredWeapon StoredWeapon, int32 Index, bool FirstEmpty, bool IntoInventory);

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerMoveInventoryItem(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter, int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter);
		void ServerMoveInventoryItem_Implementation(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter, int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter);
		bool ServerMoveInventoryItem_Validate(AInventoryItemBase* GrabbedItem, AShooterCharacterBase* SendingCharacter, int32 TargetIndex, EInventorySlotType TargetType, AShooterCharacterBase* ReceivingCharacter);

		UFUNCTION(Server, Unreliable, WithValidation)
		void ServerInteractWith(AInteractableActor* InteractActor);
		void ServerInteractWith_Implementation(AInteractableActor* InteractActor);
		bool ServerInteractWith_Validate(AInteractableActor* InteractActor);

		UFUNCTION(Server, Unreliable, WithValidation)
		void ServerSetMuzzleLookAt(FVector LookAtPoint);
		void ServerSetMuzzleLookAt_Implementation(FVector LookAtPoint);
		bool ServerSetMuzzleLookAt_Validate(FVector LookAtPoint);

		UFUNCTION(Client, Reliable, WithValidation)
		void ClientMakeNewCombatText(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit);
		void ClientMakeNewCombatText_Implementation(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit);
		bool ClientMakeNewCombatText_Validate(const FString& CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit);

		/** Function that handles the inventory items for the equipped weapons.
		*	Creates/Destroys inventory items. These inventory items represents the equipped weapons.
		*/
		virtual void UpdateEquippedInvWeapons(bool SwappedWeapons) override;

		/** Refresh the structs on the inv items that represents different equipments. (Current ammo) */
		virtual void RefreshEquippedInvWeapons() override;

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
