// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/BaseWeapon.h"

#include "Engine/DataTable.h"

#include "GameFramework/Actor.h"
#include "EquipmentManager.generated.h"

/** Structure used for data table that lists all base weapons
*	Different from FWeaponAttributes in BaseWeapon.h since this is only used for the data table to store the base weapons stats.
*	FWeaponAttributes for the base stats on the weapon is set using these values.
*	FWeaponAttributes contains the base stats and scaled stats of the weapon.
*/
USTRUCT(BlueprintType)
struct FWeaponBases : public FTableRowBase
{
	GENERATED_BODY()

	// The class of the base blueprint weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	TSubclassOf<ABaseWeapon> WeaponClass;

	// The rarity of the weapon. Rarity affects the amount of mods the weapon can have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	ERarity Rarity;

	// Which type of damage the weapon has. Different types have different effects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	EDamageTypes DamageType;

	// The damage range of this weapon, can roll value between Low and High when dealing damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	int32 DamageLow;

	// The damage range of this weapon, can roll value between Low and High when dealing damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	int32 DamageHigh;

	// The crit chance of the weapon, a crit hit is increased by the crit damage variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	float CritChance;

	// How much the crits should increase the damage done. This value is a multiplier, 20% crit damage = 1.2f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	float CritDamage;

	// How long it takes to reload the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	float ReloadSpeed;

	// The fire reate of the weapon, defined in rounds per second. 3 projectiles per second = 0.33 delay between projectiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	float FireRate;

	// The speed of the fired projectiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	float ProjectileSpeed;

	// How many rounds the weapon can hold, when all has been fired, required reload.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	int32 MagazineSize;

	// How many projectiles that are spawned each FireWeapon call
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	int32 ProjectileAmount;

	// The radius of the explosion, if there is any
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	float ExplosionRadius;

	// If the weaopn base can be randomly selected for the generate weapon stats function. False for legendary base weaopns.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	bool CanBeRandomlySelected;

	// If the weapon should have pre-set mods.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	FString PreSetMods;

	// Icon that will be used in inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base")
	UTexture2D* EquipmentIcon;

	FWeaponBases()
	{
		WeaponClass = nullptr;
		EquipmentIcon = nullptr;
	}
};

/** Struct used for the data table that contains all the modifiers that weapons can roll.
*/
USTRUCT(BlueprintType)
struct FWeaponModifierRow : public FTableRowBase
{
	GENERATED_BODY()

	// The value that is modified by the value range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
	EWeaponStats StatToModify;

	/*	Modified value range, low. Mod value is randomized between low and high.
	*	The value is handled differently depending on mod type
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
	float RangeLow;

	/*	Modified value range, high. Mod value is randomized between low and high.
	*	The value is handled differently depending on mod type
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
	float RangeHigh;

	// Not all weapon can use veery mod, this field specifies which types can use htis mod. ALL for all weapons.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifiers")
	FString ValidWeaponTypes;
};

/** Class used when calling function to generate weapon stats
*	Requires rarity to be set. Rest can be left false to randomly generate.
*/
USTRUCT(BlueprintType)
struct FGenerationParameters
{
	GENERATED_BODY()

	// The rarity the weapon should have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	ERarity Rarity;

	// If the generation should pick a certain weapon, weapon gotten by name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	FName EquipmentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	bool UseEquipmentName;

	// If the generation should pick a specific damage type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	EDamageTypes DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	bool UseDamageType;

	// If the generation should pick a specific weapon type. EquipmentName has higher priority
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	EWeaponTypes WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Parameters")
	bool UseWeaponType;
};

USTRUCT(BlueprintType)
struct FRarityModAmounts
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Mod Amount")
	ERarity Rarity;

	// How many mods this rarity can roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Mod Amount")
	int32 ModAmount;
};


UCLASS()
class TDSHOOTER_API AEquipmentManager : public AActor
{
	GENERATED_BODY()
	
	public:	
		
		AEquipmentManager();

		/** Data table reference that contains the base weapons */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Manager")
		UDataTable* BaseWeaponsDataTable;

		/** Data table reference that contains all mods the weapons can roll */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Manager")
		UDataTable* ModifiersDataTable;

		/** How many mods each rarity will have */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Manager")
		TArray<FRarityModAmounts> ModAmounts;


		//--- Weapon generation and creating ---//

		/**	This function generates a random weapon and returns the stat struct with the generated stats.
		*	Takes a struct as parameters with information about how the stats should be generated.
		*	The weapon that is generated is random. But it cannot randomize pre-set legendary weapons.
		*	Altough it can roll new legendary weapons.
		*	Function can handle pre-set stats, but these take a slot and can generate on common weapons.
		*/
		UFUNCTION(BlueprintCallable, Category = "Equipment Manager")
		FStoredWeapon GenerateWeaponStats(FGenerationParameters GenerationParam, bool& Success);

		/** This function generates a pre-set weaopn and returns the stat struct with the stats
		*	Takes a name to the weapon that should be generated.
		*	All the stats and mods will be taken from data tables. Nothing random.
		*	Can be used on all weapon classes, to get the pure weapon, no random stats on it.
		*	Useful for Legendary pre-set weapons.
		*/
		UFUNCTION(BlueprintCallable, Category = "Equipment Manager")
		FStoredWeapon GeneratePreSetWeaponStats(FName EquipmentName, bool& Success);

		/** Create the weapon in the world, returns a reference that can be equipped by characters
		*	Creates the weapon based on the StoredWeapon struct.
		*/
		UFUNCTION(BlueprintCallable, Category = "Equipment Manager")
		ABaseWeapon* CreateStoredWeapon(FStoredWeapon StoredWeapon);

		/** Gets the base stats from the base weapon data table and fills the base stat struct and returns it */
		UFUNCTION(BlueprintCallable, Category = "Equipment Manager")
		FWeaponAttributes GetWeaponBaseStats(FName EquipmentName);


		//--- Mods ---//

		/** Generates an array with modifiers and returns them.
		*	Modifiers are gathered from the data table.
		*	Amount of modifiers are dependant on the rarity of the weapon.
		*	Also sets any pre-set mods from the data table
		*/
		UFUNCTION(BlueprintCallable, Category = "Equipment Manager")
		TArray<FWeaponModifier> GenerateWeaponModifiers(EWeaponTypes WeaponType, ERarity Rarity, FWeaponBases WeaponBaseRow, bool& Success);
		

	protected:


		//--- Mods ---//

		/** Returns the preset mods from the data table row */
		TArray<FWeaponModifier> GetWeaponPreSetMods(FWeaponBases WeaponBaseRow);

		/** Finds a single random mod and returns it */
		FWeaponModifier FindWeaponMod(EWeaponTypes WeaponType, TArray<FWeaponModifierRow*> UsableRows, bool& Success);

		/** Array that contains all of the rows in the weapon modifier data table, fillers removed. */
		TArray<FWeaponModifierRow*> WeaponModifierRows;

		/** Array that contains all of the row names in the modifier data table, fillers removed */
		TArray<FName> WeaponModifierRowNames;


		//--- Helper ---//

		/** Sends in the weapon type and returns the short name version of that weapon type */
		FString ShortenedWeaponType(EWeaponTypes WeaponType);
		EWeaponTypes ShortenedWeaponTypeBack(FString ShortenedType);

		/** Gets enum to string */
		FString WeaponStatAsString(EWeaponStats WeaponStat);

		/** Functions used to get weapon base data table rows */
		FWeaponBases* GetRandomRow(int32& Index);
		FWeaponBases* GetRandomRowFromType(EWeaponTypes Type, int32& Index);


		//--- Weapon & Stat ---//

		/** Scale the provided stats with the modifiers. Also set rarity and damage type on the scaled stats */
		FWeaponAttributes ScaleWeaponStats(FWeaponAttributes BaseStats, TArray<FWeaponModifier> Modifiers, ERarity NewRarity, EDamageTypes NewDamageType);

		/** All the row names in the weapon bases data table, except the filler "-----" */
		TArray<FName> WeaponBaseRowNames;


		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaTime) override;
};
