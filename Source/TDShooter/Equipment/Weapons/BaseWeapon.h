// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Global/ShooterStaticLibrary.h"
#include "Global/DamageColorData.h"

#include "Projectiles/BaseProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "Equipment/BaseEquipment.h"

#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

/** Keeps all the weaopns attributes collected in one container */
USTRUCT(BlueprintType)
struct FWeaponAttributes
{
	GENERATED_BODY()

	// The rarity of the weapon. Rarity affects the amount of mods the weapon can have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	ERarity Rarity;

	// Which type of damage the weapon has. Different types have different effects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	EDamageTypes DamageType;

	// The damage range of this weapon, can roll value between Low and High when dealing damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 DamageLow;

	// The damage range of this weapon, can roll value between Low and High when dealing damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 DamageHigh;

	// The crit chance of the weapon, a crit hit is increased by the crit damage variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float CritChance;

	// How much the crits should increase the damage done. This value is a multiplier, 20% crit damage = 1.2f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float CritDamage;

	// How long it takes to reload the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float ReloadSpeed;

	// The fire reate of the weapon, defined in rounds per second. 3 projectiles per second = 0.33 delay between projectiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float FireRate;

	// The speed of the fired projectiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float ProjectileSpeed;

	// How many rounds the weapon can hold, when all has been fired, required reload.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 MagazineSize;

	// How many projectiles that are spawned each FireWeapon call
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 ProjectileAmount;

	// The radius of the explosion, if there is any
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float ExplosionRadius;

	FWeaponAttributes()
	{
		Rarity = ERarity::R_Common;
		DamageType = EDamageTypes::DT_Fire;
		DamageLow = 0;
		DamageHigh = 0;
		CritChance = 0.0f;
		CritDamage = 0.0f;
		ReloadSpeed = 0.0f;
		FireRate = 0.0f;
		ProjectileSpeed = 0.0f;
		MagazineSize = 0;
		ProjectileAmount = 0;
		ExplosionRadius = 0.0f;
	}
};

/** All the different stats that can be modified on a weapon */
UENUM(BlueprintType)
enum class EWeaponStats : uint8
{
	WS_None				UMETA(DisplayName = "None"),
	WS_IncreasedDamage	UMETA(DisplayName = "IncreasedDamage"),
	WS_CritChance		UMETA(DisplayName = "CritChance"),
	WS_CritDamage		UMETA(DisplayName = "CritDamage"),
	WS_ReloadSpeed		UMETA(DisplayName = "ReloadSpeed"),
	WS_FireRate			UMETA(DisplayName = "FireRate"),
	WS_ProjectileSpeed	UMETA(DisplayName = "ProjectileSpeed"),
	WS_MagazineSize		UMETA(DisplayName = "MagazineSize"),
	//WS_ProjectileAmount	UMETA(DisplayName = "ProjectileAmount"),
	WS_ExplosionRadius	UMETA(DisplayName = "ExplosionRadius")
};

USTRUCT(BlueprintType)
struct FWeaponModifier
{
	GENERATED_BODY()

	// The class of the base blueprint weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Modifier")
	EWeaponStats ModifiedStat;

	// The value the modified stat will be modified by, different logic for different stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Modifier")
	float ModifierValue;

	// Display string to be shown on the weaopn tooltips, (20% Increased Fire Rate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Modifier")
	FString ModifierDisplayString;

	bool operator==(const FWeaponModifier& OtherItem) const
	{
		return (ModifiedStat == OtherItem.ModifiedStat) && (ModifierValue == OtherItem.ModifierValue);
	}
};

// Let FStoredWeapon know the class exist, as it needs to reference it.
class ABaseWeapon;

/** The storage struct for weapons. This will contain everything needed to recreate a weapon.
*	Sending this struct to the weaopn manager will return the created weapon.
*	Use GetStoredWeapon() on base weapon to get the accurate AmmoCountCurrent value
*/
USTRUCT(BlueprintType)
struct FStoredWeapon
{
	GENERATED_BODY()

	// The class of the base blueprint weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	TSubclassOf<ABaseWeapon> WeaponClass;

	// What name this weapon has. Used for getting base stats from data table and displaying to player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	FName EquipmentName;

	// What weapon type the weapon is.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	EWeaponTypes WeaponType;

	// The rarity of the weapon. Rarity affects the amount of mods the weapon can have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	ERarity Rarity;

	// Which type of damage the weapon has. Different types have different effects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	EDamageTypes DamageType;

	// The base stats of the weaopns, before scaling from mods.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	FWeaponAttributes BaseStats;

	// The scaled stats of the weaopns, after scaling from mods.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	FWeaponAttributes ScaledStats;

	// The modifiers the weapon has, these are used to scale the base stats of the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	TArray<FWeaponModifier> Modifiers;

	// How much ammo the weapon has currently.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	int32 AmmoCountCurrent;

	// Icon that will be used in inventory 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stored Weapon")
	UTexture2D* EquipmentIcon;

	bool operator==(const FStoredWeapon& OtherItem) const
	{
		return (WeaponClass == OtherItem.WeaponClass) && (EquipmentName == OtherItem.EquipmentName)
				&& (WeaponType == OtherItem.WeaponType) && (Rarity == OtherItem.Rarity) && (DamageType == OtherItem.DamageType)
				&& (WeaponType == OtherItem.WeaponType) && (Modifiers == OtherItem.Modifiers) && (AmmoCountCurrent == OtherItem.AmmoCountCurrent);
	}

	FStoredWeapon()
	{
		WeaponClass = nullptr;
		Rarity = ERarity::R_Common;
		DamageType = EDamageTypes::DT_Fire;
		EquipmentIcon = nullptr;
	}
};

UCLASS()
class TDSHOOTER_API ABaseWeapon : public ABaseEquipment
{
	GENERATED_BODY()
	
	public:	

		ABaseWeapon();
		
		/** The main skeletal mesh associated with this Character (optional sub-object). */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* WeaponMesh;

		/** Root component to hold all the other components */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		USceneComponent* MuzzleLocation;


		/** What weapon type this weapon is.  */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		EWeaponTypes WeaponType;

		/** A class reference to itself, needs to be set in blueprint class */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
		TSubclassOf<ABaseWeapon> WeaponClass;

		/** What projectile this weapon should be shooting. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		UClass* WeaponProjectileBP;

		/** Collection of all the different colors used for the different damage types */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		UDamageColorData* ColorData;

		/** The color of the projectiles and effects */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Base")
		FLinearColor DamageTypeColor;


		//--- Weapon Stats ---//

		/** The base stats of the weaopns, before scaling from mods. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon Stats")
		FWeaponAttributes BaseStats;

		/** These are the scaled base stats, these values are the onees that are used by the weapon. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon Stats")
		FWeaponAttributes ScaledStats;

		/** This array contains the stat modifiers the weapon has */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon Stats")
		TArray<FWeaponModifier> Modifiers;

		/** How many rounds the weapon currently has */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon Stats")
		int32 AmmoCountCurrent;

		/** How many rounds the weapon currently has */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon Stats")
		float ReloadDelay;


		//--- Functions ---//

		/** General function to Fire the weapon, requires override in subclass. Cal Super:: if no own implementation is made */
		UFUNCTION(BlueprintCallable, Category = "Base")
		virtual bool FireWeapon(AActor* InstigatorActor, EFactions Faction);

		/** General function that spawns the projectiles. Called from FireWeapon() */
		UFUNCTION(BlueprintCallable, Category = "Base")
		bool SpawnProjectile(int32 InProjectileAmount, float InProjectileSpeed, UClass* ProjectileBP, AActor* IgnoreActor, EFactions Faction);

		/** Should be called when reloading the weapon for animations and time until player can fire again  */
		UFUNCTION(BlueprintCallable, Category = "Base")
		void ReloadWeapon();

		/** Blueprint event that is called after the weapon has been fired. */
		UFUNCTION(BlueprintImplementableEvent, Category = "Base")
		void BPFiredWeapon();

		/** Blueprint event that is called after the weapon has been reloaded. */
		UFUNCTION(BlueprintImplementableEvent, Category = "Base")
		void BPReloadWeapon();

		/* Checks the fire rate delay and reload delay if the gun can be fired. Does not check for ammo. */
		UFUNCTION(BlueprintPure, Category = "Base")
		bool CanFire();
		
		/** Builds and returns a StoredWeapon struct that contains all needed variables to re-create the weapon. */
		UFUNCTION(BlueprintCallable, Category = "Base")
		FStoredWeapon GetStoredWeapon(bool& Success);


		/** Gets the base stats from the base weapon data table and fills the base stat struct */
		void SetBaseStats(FWeaponAttributes NewBaseStats);
	
		/** Sets all the modifiers on this weapon. Requires ScaleBaseStats() to take effect */
		void SetStatModifiers(TArray<FWeaponModifier> NewModifiers);

		/** Sets the scaled stats to the stats provided by the weapon manager, also sets the correct colors */
		void SetScaledStats(FWeaponAttributes NewScaledStats);

		/** Set the rotation of the muzzle scene component on the server to face a certain position */
		void SetMuzzleLookAt(FVector LookAtPoint);

		
		UPROPERTY(Replicated)
		float FireDelay;


	protected:
		
		UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void MulticastFireWeaponEffect();
		void MulticastFireWeaponEffect_Implementation();
		bool MulticastFireWeaponEffect_Validate();

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
};
