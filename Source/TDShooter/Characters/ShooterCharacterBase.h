// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Global/ShooterStaticLibrary.h"
#include "Equipment/Weapons/BaseWeapon.h"
#include "UnrealNetwork.h"
#include "Inventory/InventoryItemWeapon.h"

#include "GameFramework/Character.h"
#include "ShooterCharacterBase.generated.h"

UCLASS()
class TDSHOOTER_API AShooterCharacterBase : public ACharacter
{
	GENERATED_BODY()

	public:
		
		// Sets default values for this character's properties
		AShooterCharacterBase();

		/** If this character should debug print */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Character")
		bool ShouldPrintInfo;

		/** The current faction the character has, Set using SetFaction() */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CollisionRespone, Category = "Base Character")
		EFactions Faction;

		/** The faction this character should start out as. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Character")
		EFactions InitialFaction;

		/** The initial mesh location */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Character")
		FVector InitialMeshLoc;

		/** The initial mesh location */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Character")
		FRotator InitialMeshRot;


		//--- Movement Variables ---//

		/** Enables jump animation on the character mesh */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Base Character")
		bool EnableJumpAnim;


		//--- Weapon Variables ---//

		/** Reference to the currently equipped weapon. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
		ABaseWeapon* EquippedWeapon;

		/** Reference to the secondary equipped weapon  */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
		ABaseWeapon* SecondaryEquippedWeapon;

		/** Equips the character with a new weapon in their hands.
		*	Function also destroy the previously equipped weapon if there is one.
		*	For Slot, 0 for main weapon, 1 for secondary
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool EquipWeapon(ABaseWeapon* NewWeapon, int32 Slot = 0);

		UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool FireWeapon();

		/** If the character has infinite ammo to reload with */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool InfiniteAmmo;

		/** Stores all the character ammo in a single place. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FAmmoContainer AmmoContainer;

		/** Reload animation montage to play when reloading */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* ReloadMontage;

		/** Reloads the currently equipped weapon */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool ReloadWeapon();

		/** Switch the EquippedWeapon and SecondaryEquippedWeapons */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool SwitchEquippedWeapons();


		//--- Health Variables ---//

		/** The current health of the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
		float HealthCurrent;

		/** The maximum health the character can have */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthMax;

		/** If the character is dead. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CharacterDead, Category = "Health")
		bool CharacterDead;

		/** If the character should start ragdolling when dying. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		bool ShouldRadgollOnDeath;

		/** Sets if the character is dead or not. */
		UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCharacterDead(bool IsDead);


		//--- General Functions ---//

		UFUNCTION(BlueprintCallable, Category = "Base Character")
		void SetEnableJumpAnim(bool NewValue);

		/** Sets a new faction to the character, and sets the collision to match the projectiles. 
		*	Always set this while on the server. It will replicate down to clients.
		*/
		UFUNCTION(BlueprintCallable, Category = "Base Character")
		void SetFaction(EFactions NewFaction);

		/** Rotate the character towards direction, calls server function if client */
		void SetCharacterRot(FVector Direction);


	protected:

		UFUNCTION()
		void OnRep_CollisionRespone();

		UFUNCTION()
		void OnRep_CharacterDead();

		// Multicast reload animation functions
		void PlayReloadAnim();
		UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void MulticastPlayReloadAnim();
		void MulticastPlayReloadAnim_Implementation();
		bool MulticastPlayReloadAnim_Validate();

		//--- Server Functions ---//

		/** Sets the enable jump anim bool on the server */
		UFUNCTION(Server, Unreliable, WithValidation)
		void ServerSetEnableJumpAnim(bool NewValue);
		void ServerSetEnableJumpAnim_Implementation(bool NewValue);
		bool ServerSetEnableJumpAnim_Validate(bool NewValue);

		/** Set Rotation of character on server. */
		UFUNCTION(Server, Unreliable, WithValidation)
		void ServerSetCharacterRot(FVector Direction);
		void ServerSetCharacterRot_Implementation(FVector Direction);
		bool ServerSetCharacterRot_Validate(FVector Direction);

		/** Fire the weapon on the server */
		UFUNCTION(Server, Reliable, WithValidation)
		void ServerFireWeapon();
		void ServerFireWeapon_Implementation();
		bool ServerFireWeapon_Validate();

		/** Reload the weapon on the server */
		UFUNCTION(Server, Reliable, WithValidation)
		void ServerReloadWeapon();
		void ServerReloadWeapon_Implementation();
		bool ServerReloadWeapon_Validate();

		/** Reload the weapon on the server */
		UFUNCTION(Server, Reliable, WithValidation)
		void ServerSwitchEquippedWeapons();
		void ServerSwitchEquippedWeapons_Implementation();
		bool ServerSwitchEquippedWeapons_Validate();

		virtual void UpdateEquippedInvWeapons(bool SwappedWeapons);
		virtual void RefreshEquippedInvWeapons();

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

};
