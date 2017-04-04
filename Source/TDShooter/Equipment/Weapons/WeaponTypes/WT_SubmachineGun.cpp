// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "WT_SubmachineGun.h"

AWT_SubmachineGun::AWT_SubmachineGun()
{
	// Set the weapon type.
	WeaponType = EWeaponTypes::WT_SubmachineGun;
}

// Called when the game starts or when spawned
void AWT_SubmachineGun::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWT_SubmachineGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AWT_SubmachineGun::FireWeapon(AActor* InstigatorActor, EFactions Faction)
{
	//return Super::FireWeapon(InstigatorActor, Faction);

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


bool AWT_SubmachineGun::SpawnSMGProjectile(int32 InProjectileAmount, float InProjectileSpeed, UClass* ProjectileBP, AActor* InstigatorActor, EFactions Faction)
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