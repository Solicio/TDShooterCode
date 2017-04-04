// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "ShooterStaticLibrary.h"

#include "../Characters/Player/ShooterCharacter.h"
//#include "../Characters/NPC/NPC_Base.h"
#include "../Characters/ShooterCharacterBase.h"

EFactions UShooterStaticLibrary::GetCharacterFaction(ACharacter* Character)
{
	if (Character == nullptr)
		return EFactions::F_None;

	if (Cast<AShooterCharacterBase>(Character))
	{
		return Cast<AShooterCharacterBase>(Character)->Faction;
	}

	return EFactions::F_None;
}


FString UShooterStaticLibrary::GetWeaponEnumAsString(EWeaponTypes EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWeaponTypes"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetEnumName((int32)EnumValue);
}

FString UShooterStaticLibrary::GetFactionEnumAsString(EFactions EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFactions"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetEnumName((int32)EnumValue);
}

FString UShooterStaticLibrary::GetDamageTypeEnumAsString(EDamageTypes EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDamageTypes"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetEnumName((int32)EnumValue);
}


float UShooterStaticLibrary::CalculateDamageCharacter(AShooterCharacterBase* CharacterHit, AShooterCharacterBase* DamageInstigator, int32 DamageLow, int32 DamageHigh, float CritChance, float CritDamage, EDamageTypes DamageType, bool& WasCrit)
{
	if (CharacterHit == nullptr)
		return 0.0f;

	float Damage; 

	// Make sure instigator is also valid, but instigator is not required to deal damage

	// Character Hit Resists * DamageType
	// Instigator Character Damage buffs/debuffs
	// Damage Range, Low - High
	// Crit Chance, Damage * CritDamage

	// The damage range
	Damage = FMath::RandRange(DamageLow, DamageHigh);

	// Crit
	float CritRoll = FMath::RandRange(0.0f, 100.0f);
	if (CritRoll <= CritChance)
	{
		WasCrit = true;

		// Multiply by crit damage
		Damage = (Damage *= (CritDamage / 100.0f));
	}

	Damage = FMath::RoundToInt(Damage);
	return Damage;
}

void UShooterStaticLibrary::ApplyDamageCharacter(AShooterCharacterBase* CharacterHit, AShooterCharacterBase* DamageInstigator, int32 Damage, EDamageTypes DamageType, bool WasCrit)
{
	if (CharacterHit == nullptr)
		return;

	// Make sure instigator is also valid, but instigator is not required to deal damage

	// Proc effects

	// Apply the damage and clamp the value
	CharacterHit->HealthCurrent -= Damage;
	CharacterHit->HealthCurrent = FMath::Clamp(CharacterHit->HealthCurrent, 0.0f, CharacterHit->HealthMax);

	// Set the character to dead if the health is or is below 0
	if (CharacterHit->HealthCurrent <= 0.0f)
		CharacterHit->SetCharacterDead(true);

	// Floating combat text
	if (CharacterHit->ActorHasTag("Player") || DamageInstigator->ActorHasTag("Player"))
	{
		// If the player was the one taking damage
		AShooterCharacter* Player = Cast<AShooterCharacter>(CharacterHit);
		if (Player)
			Player->MakeNewCombatText(FString::SanitizeFloat(Damage), Player->GetActorLocation(), DamageType, WasCrit);

		// Or if the player was the one doing the damage
		Player = Cast<AShooterCharacter>(DamageInstigator);
		if (Player)
			Player->MakeNewCombatText(FString::SanitizeFloat(Damage), CharacterHit->GetActorLocation(), DamageType, WasCrit);
	}

	//PrintText("Damage Done: " + FString::SanitizeFloat(Damage), -1, 2.0f, FColor::Red);
}