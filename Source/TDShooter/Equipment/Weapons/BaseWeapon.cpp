// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "BaseWeapon.h"

#include "Characters/ShooterCharacterBase.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(RootComponent);
	//MuzzleLocation->SetWorldRotation(FRotator(0.0f, 90.0f, 0.0f));

	AmmoCountCurrent = 0;

	// The name of the weapon
	EquipmentName = "BaseWeapon";

	// Set the type of equipment this is
	EquipmentType = EEquipmentTypes::ET_Weapon;

	bReplicates = true;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, BaseStats);
	DOREPLIFETIME(ABaseWeapon, ScaledStats);
	DOREPLIFETIME(ABaseWeapon, AmmoCountCurrent);
	DOREPLIFETIME(ABaseWeapon, FireDelay);
	DOREPLIFETIME(ABaseWeapon, ReloadDelay);
	DOREPLIFETIME(ABaseWeapon, DamageTypeColor);
	DOREPLIFETIME(ABaseWeapon, Modifiers);

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// Set the base stats to the values from the data table. SET DURING SPAWN
	/*SetBaseStats();*/

	// Scale the base stats to the scaled values using the mods rolled onto the weapon. SET DURING SPAWN
	/*ScaleBaseStats();*/
}

// Called every frame
void ABaseWeapon::Tick (float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		if (FireDelay > 0.0f)
		{
			FireDelay -= DeltaTime;

			if (FireDelay <= 0.0f)
				FireDelay = 0.0f;
		}

		if (ReloadDelay > 0.0f)
		{
			ReloadDelay -= DeltaTime;

			if (ReloadDelay <= 0.0f)
				ReloadDelay = 0.0f;
		}
	}
}

bool ABaseWeapon::FireWeapon(AActor* InstigatorActor, EFactions Faction)
{
	// This function should be called by using Super::FireWeapon.
	// This function handles ammo, rate of fire and calling the function that spawns projectiles
	
	if (WeaponProjectileBP == nullptr || FireDelay > 0.0f || AmmoCountCurrent <= 0 || ReloadDelay > 0.0f || Role < ROLE_Authority)
		return false;

	bool FiredWeapon = false;

	// Set a delay based on the fire rate of the weapon
	FireDelay = 1.0f / ScaledStats.FireRate;

	// If the current ammo count is less than the usual amount fired, shoot the remaining ammo
	if (AmmoCountCurrent < ScaledStats.ProjectileAmount)
	{
		// Spawn the projectiles.
		FiredWeapon = SpawnProjectile(AmmoCountCurrent, ScaledStats.ProjectileSpeed, WeaponProjectileBP, InstigatorActor, Faction);
		AmmoCountCurrent = 0;
	}
	else
	{
		// Spawn the projectiles.
		FiredWeapon = SpawnProjectile(ScaledStats.ProjectileAmount, ScaledStats.ProjectileSpeed, WeaponProjectileBP, InstigatorActor, Faction);
		AmmoCountCurrent -= ScaledStats.ProjectileAmount;
	}

	// Play the fire effect on the server and all clients.
	MulticastFireWeaponEffect();

	return FiredWeapon;
}

bool ABaseWeapon::SpawnProjectile(int32 InProjectileAmount, float InProjectileSpeed, UClass* ProjectileBP, AActor* InstigatorActor, EFactions Faction)
{
	if (ProjectileBP == nullptr || Role < ROLE_Authority)
		return false;

	// Create the projectile.
	ABaseProjectile* SpawnedProj = SpawnBP<ABaseProjectile>(GetWorld(), ProjectileBP, MuzzleLocation->GetComponentLocation(), FRotator(0.0f, 0.0f, 0.0f));

	if (SpawnedProj)
	{
		//Tell the projectile who spawned it.
		SpawnedProj->InstigatorCharacter = Cast<AShooterCharacterBase>(InstigatorActor);

		// Set the damage stats on the projectile. These values snapshot when spawning the projectile.
		SpawnedProj->DamageType = ScaledStats.DamageType;
		SpawnedProj->DamageLow = ScaledStats.DamageLow;
		SpawnedProj->DamageHigh = ScaledStats.DamageHigh;
		SpawnedProj->CritChance = ScaledStats.CritChance;
		SpawnedProj->CritDamage = ScaledStats.CritDamage;
		SpawnedProj->ExplosionRadius = ScaledStats.ExplosionRadius;

		// Set the instigator faction to the projectile.
		SpawnedProj->Faction = Faction;

		// Get the collision profile name
		FString ProfileName;
		if (Faction != EFactions::F_None)
			ProfileName = "Projectiles_F" + FString::FromInt((int32)SpawnedProj->Faction);

		// Set the DamageColor, DamageType and Velocity of all the projectiles. (Multicast Function)
		FVector ProjectileVelocity = MuzzleLocation->GetForwardVector() * InProjectileSpeed;
		SpawnedProj->SetInitialValues(ScaledStats.DamageType, DamageTypeColor, ProjectileVelocity, *ProfileName);

		return true;
	}

	// Return false if the projctile was invalid
	return false;
}

void ABaseWeapon::MulticastFireWeaponEffect_Implementation()
{
	BPFiredWeapon();
}

bool ABaseWeapon::MulticastFireWeaponEffect_Validate()
{
	return true;
}

bool ABaseWeapon::CanFire()
{
	return (FireDelay <= 0.0f) && (ReloadDelay <= 0.0f);
}

void ABaseWeapon::ReloadWeapon()
{
	ReloadDelay = ScaledStats.ReloadSpeed;
	BPReloadWeapon();
}

FStoredWeapon ABaseWeapon::GetStoredWeapon(bool& Success)
{
	FStoredWeapon StoredWeapon;
	if (WeaponClass == nullptr)
	{
		Success = false;
		return StoredWeapon;
	}

	// Fills a StoredWeapon struct with all variables.
	StoredWeapon.WeaponClass = WeaponClass;
	StoredWeapon.EquipmentName = EquipmentName;
	StoredWeapon.WeaponType = WeaponType;
	StoredWeapon.Rarity = ScaledStats.Rarity;
	StoredWeapon.DamageType = ScaledStats.DamageType;
	StoredWeapon.BaseStats = BaseStats;
	StoredWeapon.ScaledStats = ScaledStats;
	StoredWeapon.Modifiers = Modifiers;
	StoredWeapon.AmmoCountCurrent = AmmoCountCurrent;
	StoredWeapon.EquipmentIcon = EquipmentIcon;
	
	// And send it back.
	Success = true;
	return StoredWeapon;
}

void ABaseWeapon::SetBaseStats(FWeaponAttributes NewBaseStats)
{
	// Set the base stats to the new stats
	BaseStats = NewBaseStats;
}

void ABaseWeapon::SetStatModifiers(TArray<FWeaponModifier> NewModifiers)
{
	// Set the modifiers
	Modifiers = NewModifiers;
}

void ABaseWeapon::SetScaledStats(FWeaponAttributes NewScaledStats)
{
	ScaledStats = NewScaledStats;

	// Set the rarity and damage type from the weapon manager by input parameters
	//ScaledStats.Rarity = Rarity;
	//ScaledStats.DamageType = DamageType;

	// Set the color of the effects from the weapon to match damage type.
	switch (ScaledStats.DamageType)
	{
		case EDamageTypes::DT_Fire:
			DamageTypeColor = ColorData->Colors.Fire;
			break;

		case EDamageTypes::DT_Ice:
			DamageTypeColor = ColorData->Colors.Ice;
			break;

		case EDamageTypes::DT_Poison:
			DamageTypeColor = ColorData->Colors.Poison;
			break;

		case EDamageTypes::DT_Lightning:
			DamageTypeColor = ColorData->Colors.Lightning;
			break;

		case EDamageTypes::DT_Arcane:
			DamageTypeColor = ColorData->Colors.Arcane;
			break;
	};
}

void ABaseWeapon::SetMuzzleLookAt(FVector LookAtPoint)
{
	// No point in setting this while on a client. As the client will never fire a projectile.
	if (Role < ROLE_Authority)
		return;

	LookAtPoint.Z = MuzzleLocation->GetComponentLocation().Z;
	float DistanceToMouse = (MuzzleLocation->GetComponentLocation() - LookAtPoint).Size();

	if (DistanceToMouse >= 100.0f)
	{
		// Get the look at rotation to the location and set the muzzle to rotate towards it
		FRotator MuzzleRot = UKismetMathLibrary::MakeRotFromX(LookAtPoint - MuzzleLocation->GetComponentLocation());
		MuzzleLocation->SetWorldRotation(MuzzleRot);
	}
	else
	{
		// Reset if the mouse is too close
		MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}
}