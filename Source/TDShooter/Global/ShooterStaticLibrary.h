// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "ShooterStaticLibrary.generated.h"

/** The different weapon types in the game. */
UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	WT_None				UMETA(DisplayName = "None"),
	WT_AssaultRifle		UMETA(DisplayName = "AssualtRifle"),
	WT_SubmachineGun	UMETA(DisplayName = "SubmachineGun"),
	WT_RocketLauncher	UMETA(DisplayName = "RocketLauncher"),
	WT_GrenadeLauncher	UMETA(DisplayName = "GrenadeLauncher"),
};


/** The different factions in the game.
*	Used to determine which NPC are hostile to the player or to eachother.
*	Player Default faction is Faction 1
*/
UENUM(BlueprintType)
enum class EFactions : uint8
{
	F_None		UMETA(DisplayName = "None"),
	F_Faction1	UMETA(DisplayName = "Faction 1"),
	F_Faction2	UMETA(DisplayName = "Faction 2"),
	F_Faction3	UMETA(DisplayName = "Faction 3"),
	F_Faction4	UMETA(DisplayName = "Faction 4")
};

/** The different types of damage that weapons can have. Each with their own special effects. Except for the normal damage type. */
UENUM(BlueprintType)
enum class EDamageTypes : uint8
{
	DT_Fire			UMETA(DisplayName = "Fire"),
	DT_Poison		UMETA(DisplayName = "Poison"),
	DT_Ice			UMETA(DisplayName = "Ice"),
	DT_Lightning	UMETA(DisplayName = "Lightning"),
	DT_Arcane		UMETA(DisplayName = "Arcane")
};

/** The different rarity levels */
UENUM(BlueprintType)
enum class ERarity : uint8
{
	R_Common		UMETA(DisplayName = "Common"),
	R_Uncommon		UMETA(DisplayName = "Uncommon"),
	R_Rare			UMETA(DisplayName = "Rare"),
	R_Epic			UMETA(DisplayName = "Epic"),
	R_Legendary		UMETA(DisplayName = "Legendary")
};

/** The different types of inventory slots */
UENUM(BlueprintType)
enum class EInventorySlotType : uint8
{
	IST_Inventory		UMETA(DisplayName = "Inventory"),
	IST_Stash			UMETA(DisplayName = "Stash"),
	IST_Shop			UMETA(DisplayName = "Shop"),
	IST_Equipped		UMETA(DisplayName = "Equipped")
};

/** Used to keep track of the different weapon type ammo */
USTRUCT(BlueprintType)
struct FAmmoType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	int32 AmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	EWeaponTypes AmmoType;
};

/** Stores all different ammo types into one struct for easy handling */
USTRUCT(BlueprintType)
struct FAmmoContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	TArray<FAmmoType> AmmoTypes;

	FAmmoContainer()
	{
		FAmmoType TempType;
		TempType.AmmoCount = 0;

		TempType.AmmoType = EWeaponTypes::WT_SubmachineGun;
		AmmoTypes.Add(TempType);

		TempType.AmmoType = EWeaponTypes::WT_AssaultRifle;
		AmmoTypes.Add(TempType);

		TempType.AmmoType = EWeaponTypes::WT_RocketLauncher;
		AmmoTypes.Add(TempType);

	}
};

#define COLLISION_PROJ_F1	ECC_GameTraceChannel1
#define COLLISION_PROJ_F2	ECC_GameTraceChannel2
#define COLLISION_PROJ_F3	ECC_GameTraceChannel3
#define COLLISION_PROJ_F4	ECC_GameTraceChannel4

#define CHANNEL_INTERACT_TRACE	ECC_GameTraceChannel5

class AShooterCharacterBase;

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UShooterStaticLibrary : public UObject
{
	GENERATED_BODY()

	public:

		/** Returns the faction of the provided character, only works for NPC_Base and ShooterCharacter characters */
		static EFactions GetCharacterFaction(ACharacter* Character);

		/** Functions to get enums to string*/
		static FString GetWeaponEnumAsString(EWeaponTypes EnumValue);
		static FString GetFactionEnumAsString(EFactions EnumValue);
		static FString GetDamageTypeEnumAsString(EDamageTypes EnumValue);

		//---- Damage functions ----//

		/** Calculates how much damage will be done on hit to a certain enemy.
		*	Takes parameter of the character that fired the weapon and the character that was hit.
		*	Also takes the weapons stats that can affect the damage done.
		*	Returns how much damage should be applied to the hit character.
		*/
		static float CalculateDamageCharacter(AShooterCharacterBase* CharacterHit, AShooterCharacterBase* DamageInstigator, int32 DamageLow, int32 DamageHigh, float CritChance, float CritDamage, EDamageTypes DamageType, bool& WasCrit);

		/** This function applies the damage that has been calculated onto the hit character.
		*	Sets the character to dead if health reaches 0.
		*	Takes parameter for Character that fired projectiles, the character that was hit and how much damage.
		*	Damage Instigator is not necessary to deal damage.
		*
		*	Should handle proc effects when dealing damage.
		*/
		static void ApplyDamageCharacter(AShooterCharacterBase* CharacterHit, AShooterCharacterBase* DamageInstigator, int32 Damage, EDamageTypes DamageType, bool WasCrit);


		static FORCEINLINE FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
		{
			//Round to integral if have something like 1.9999 within precision
			float Rounded = roundf(TheFloat);
			if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
			{
				TheFloat = Rounded;
			}
			FNumberFormattingOptions NumberFormat;					
			NumberFormat.UseGrouping = false;
			NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
			NumberFormat.MaximumIntegralDigits = 10000;
			NumberFormat.MinimumFractionalDigits = Precision;
			NumberFormat.MaximumFractionalDigits = Precision;
			return FText::AsNumber(TheFloat, &NumberFormat).ToString();
		}
};
