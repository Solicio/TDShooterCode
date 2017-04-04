// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "EquipmentManager.h"


// Sets default values
AEquipmentManager::AEquipmentManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Creater an array to keep track of how many mods a rarity can roll
	FRarityModAmounts NewAmount;

	NewAmount.Rarity = ERarity::R_Common;
	NewAmount.ModAmount = 0;
	ModAmounts.Add(NewAmount);

	NewAmount.Rarity = ERarity::R_Uncommon;
	NewAmount.ModAmount = 1;
	ModAmounts.Add(NewAmount);

	NewAmount.Rarity = ERarity::R_Rare;
	NewAmount.ModAmount = 2;
	ModAmounts.Add(NewAmount);

	NewAmount.Rarity = ERarity::R_Epic;
	NewAmount.ModAmount = 3;
	ModAmounts.Add(NewAmount);

	NewAmount.Rarity = ERarity::R_Legendary;
	NewAmount.ModAmount = 4;
	ModAmounts.Add(NewAmount);

}

// Called when the game starts or when spawned
void AEquipmentManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (BaseWeaponsDataTable)
	{
		// Gather all the row names from the data table
		WeaponBaseRowNames = BaseWeaponsDataTable->GetRowNames();

		// Loop through the array and remove fillers
		for (int i = WeaponBaseRowNames.Num() - 1; i >= 0; i--)
		{
			if (WeaponBaseRowNames[i].ToString().Contains("-"))
				WeaponBaseRowNames.RemoveAt(i);
		}
	}

	if (ModifiersDataTable)
	{
		// Get all the rows in the data table
		FString ContextString;
		ModifiersDataTable->GetAllRows<FWeaponModifierRow>(ContextString, WeaponModifierRows);

		// Loop through the array and remove fillers
		for (int i = WeaponModifierRows.Num() - 1; i >= 0; i--)
		{
			if (WeaponModifierRows[i]->StatToModify == EWeaponStats::WS_None)
				WeaponModifierRows.RemoveAt(i);
		}


		// Gather all the row names from the data table
		WeaponModifierRowNames = ModifiersDataTable->GetRowNames();

		// Loop through the array and remove fillers
		for (int i = WeaponModifierRowNames.Num() - 1; i >= 0; i--)
		{
			if (WeaponModifierRowNames[i].ToString().Contains("-"))
				WeaponModifierRowNames.RemoveAt(i);
		}
	}
}

// Called every frame
void AEquipmentManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FStoredWeapon AEquipmentManager::GenerateWeaponStats(FGenerationParameters GenerationParam, bool& Success)
{
	// New struct that is filled with data and then returned
	FStoredWeapon GeneratedStats;

	FString ContextString;
	FWeaponBases* SelectedRow = nullptr;
	int32 WeaponIndex = 0;

	// Selecting weapon data table row
	// By provided EquipmentName
	if (GenerationParam.UseEquipmentName)
	{
		for (int i = 0; i < WeaponBaseRowNames.Num(); i++)
		{
			// Check the row names for a match with the provided weapon name
			if (WeaponBaseRowNames[i].ToString().Contains(GenerationParam.EquipmentName.ToString()))
			{
				SelectedRow = BaseWeaponsDataTable->FindRow<FWeaponBases>(WeaponBaseRowNames[i], ContextString);
				WeaponIndex = i;
				break;
			}
		}
	}
	// By provided weapon type, specific weapon type, random weapon by that type
	else if (GenerationParam.UseWeaponType)
	{
		SelectedRow = GetRandomRowFromType(GenerationParam.WeaponType, WeaponIndex);
	}
	// Completely random weapon
	else
	{
		SelectedRow = GetRandomRow(WeaponIndex);
	}

	// Row is valid and class is valid, set the various variables, such as EquipmentName, Type, BaseStats, DamageType, Rarity and roll mods.
	if (SelectedRow && SelectedRow->WeaponClass)
	{
		GeneratedStats.WeaponClass = SelectedRow->WeaponClass;

		// Split the row name to get the weapon name and type
		FString RowName = WeaponBaseRowNames[WeaponIndex].ToString();
		FString WeaponTypeString;
		FString EquipmentNameString;
		RowName.Split("$", &WeaponTypeString, &EquipmentNameString);

		// Set the name and type
		GeneratedStats.EquipmentName = *EquipmentNameString;
		GeneratedStats.WeaponType = ShortenedWeaponTypeBack(WeaponTypeString);

		// The weapon base stats
		GeneratedStats.BaseStats = GetWeaponBaseStats(GeneratedStats.EquipmentName);
		GeneratedStats.AmmoCountCurrent = GeneratedStats.BaseStats.MagazineSize;

		// Selecting rarity, rarity should be selected in the loot system based of rarity weights, etc..
		// OR send in weights and select rarity here
		GeneratedStats.Rarity = GenerationParam.Rarity;

		// Damage type
		if (GenerationParam.UseDamageType)
		{
			GeneratedStats.DamageType = GenerationParam.DamageType;
		}
		else
		{
			// Randomly select a damage type for the weapon.
			GeneratedStats.DamageType = EDamageTypes(FMath::RandRange(0, 4));
		}

		// Generate the stat modifiers, also checks and adds pre-set mods if there are any
		bool GeneratedModifiers = false;
		GeneratedStats.Modifiers = GenerateWeaponModifiers(GeneratedStats.WeaponType, GeneratedStats.Rarity, *SelectedRow, GeneratedModifiers);

		// Set the equipment icon
		GeneratedStats.EquipmentIcon = SelectedRow->EquipmentIcon;
	}
	// Either row is not valid or the class is not valid, return the function as failed
	else if (!SelectedRow || (SelectedRow && SelectedRow->WeaponClass))
	{
		Success = false;
		return GeneratedStats;
	}

	// Scale the stats and set them in the stored weapon
	GeneratedStats.ScaledStats = ScaleWeaponStats(GeneratedStats.BaseStats, GeneratedStats.Modifiers, GeneratedStats.Rarity, GeneratedStats.DamageType);

	// Current ammo count, after scaling
	GeneratedStats.AmmoCountCurrent = GeneratedStats.ScaledStats.MagazineSize;

	Success = true;
	return GeneratedStats;
}

FStoredWeapon AEquipmentManager::GeneratePreSetWeaponStats(FName EquipmentName, bool& Success)
{
	// New struct that is filled with data and then returned
	FStoredWeapon GeneratedStats;
	FString ContextString;
	FWeaponBases* SelectedRow = nullptr;
	int32 WeaponIndex = 0;

	// Selecting weapon class based on name
	for (int i = 0; i < WeaponBaseRowNames.Num(); i++)
	{
		// Check the row names for a match with the provided weapon name
		if (WeaponBaseRowNames[i].ToString().Contains(EquipmentName.ToString()))
		{
			SelectedRow = BaseWeaponsDataTable->FindRow<FWeaponBases>(WeaponBaseRowNames[i], ContextString);
			WeaponIndex = i;
			break;
		}
	}

	// Row is valid and class is valid, set the weapon class, name, type and base stats
	if (SelectedRow && SelectedRow->WeaponClass)
	{
		GeneratedStats.WeaponClass = SelectedRow->WeaponClass;

		// Split the row name to get the weapon name and type
		FString RowName = WeaponBaseRowNames[WeaponIndex].ToString();
		FString WeaponTypeString;
		FString EquipmentNameString;
		RowName.Split("$", &WeaponTypeString, &EquipmentNameString);

		// Set the name and type
		GeneratedStats.EquipmentName = *EquipmentNameString;
		GeneratedStats.WeaponType = ShortenedWeaponTypeBack(WeaponTypeString);

		// The weapon base stats
		GeneratedStats.BaseStats = GetWeaponBaseStats(GeneratedStats.EquipmentName);

		// Set the rarity and damage type the data table
		GeneratedStats.Rarity = SelectedRow->Rarity;
		GeneratedStats.DamageType = SelectedRow->DamageType;

		// Pre-set mods
		GeneratedStats.Modifiers = GetWeaponPreSetMods(*SelectedRow);

		// Set the equipment icon
		GeneratedStats.EquipmentIcon = SelectedRow->EquipmentIcon;
	}
	// Either row is not valid or the class is not valid, return the function as failed
	else if (!SelectedRow || (SelectedRow && SelectedRow->WeaponClass))
	{
		Success = false;
		return GeneratedStats;
	}

	// Scale the stats and set them in the stored weapon
	GeneratedStats.ScaledStats = ScaleWeaponStats(GeneratedStats.BaseStats, GeneratedStats.Modifiers, GeneratedStats.Rarity, GeneratedStats.DamageType);

	// Current ammo count, after scaling
	GeneratedStats.AmmoCountCurrent = GeneratedStats.ScaledStats.MagazineSize;

	Success = true;
	return GeneratedStats;
}

ABaseWeapon* AEquipmentManager::CreateStoredWeapon(FStoredWeapon StoredWeapon)
{
	if (StoredWeapon.WeaponClass == nullptr || Role != ROLE_Authority)
		return nullptr;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the weapon in the world
	ABaseWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StoredWeapon.WeaponClass, FTransform::Identity, SpawnInfo);

	// Init it's stats
	if (SpawnedWeapon)
	{
		SpawnedWeapon->EquipmentIcon = StoredWeapon.EquipmentIcon;
		SpawnedWeapon->AmmoCountCurrent = StoredWeapon.AmmoCountCurrent;
		SpawnedWeapon->SetBaseStats(StoredWeapon.BaseStats);
		SpawnedWeapon->SetStatModifiers(StoredWeapon.Modifiers);
		SpawnedWeapon->SetScaledStats(StoredWeapon.ScaledStats);
	}

	return SpawnedWeapon;
}

#pragma region Mods

TArray<FWeaponModifier> AEquipmentManager::GenerateWeaponModifiers(EWeaponTypes WeaponType, ERarity Rarity, FWeaponBases WeaponBaseRow, bool& Success)
{
	TArray<FWeaponModifier> NewModifiers;

	if (ModifiersDataTable == nullptr || WeaponType == EWeaponTypes::WT_None)
	{
		Success = false;
		return NewModifiers;
	}

	// Get the already pre-set mods of the base weapon
	NewModifiers = GetWeaponPreSetMods(WeaponBaseRow);

	// Find how many mods that the function should generate
	int32 AmountOfMods = 0;
	for (auto Itr(ModAmounts.CreateIterator()); Itr; Itr++)
	{
		if ((*Itr).Rarity == Rarity)
		{
			// Set the mod amount based of rarity then remove num of already set mods
			AmountOfMods = (*Itr).ModAmount;
			AmountOfMods -= NewModifiers.Num();
			break;
		}
	}

	// If no mods should be rolled, just stop
	if (AmountOfMods <= 0)
	{
		Success = false;
		return NewModifiers;
	}

	// Pick out the rows that are usable by this weapon type
	TArray<FWeaponModifierRow*> UsableRows;
	for (int i = 0; i < WeaponModifierRows.Num(); i++)
	{
		// If the rows valid weapon column contains the weapon type or ALL, the row is usable
		if (WeaponModifierRows[i]->ValidWeaponTypes.Contains(ShortenedWeaponType(WeaponType)) || WeaponModifierRows[i]->ValidWeaponTypes.Contains("ALL"))
		{
			UsableRows.Add(WeaponModifierRows[i]);
		}
	}

	// First remove any mods from the usable rows array based of the pre-set mods.
	for (int k = UsableRows.Num() - 1; k >= 0; k--)
	{
		for (int l = 0; l < NewModifiers.Num(); l++)
		{
			// If the stats match, remove from usable
			if (UsableRows[k]->StatToModify == NewModifiers[l].ModifiedStat)
				UsableRows.RemoveAt(k);
		}
	}

	// Loop through the mod amounts and get new mods
	for (int i = 0; i < AmountOfMods; i++)
	{
		bool FoundMod;
		FWeaponModifier NewMod = FindWeaponMod(WeaponType, UsableRows, FoundMod);

		if (FoundMod)
		{
			NewModifiers.Add(NewMod);

			// After adding a new mod, remove all other rows that has that same weapon stat
			for (int j = UsableRows.Num() - 1; j >= 0; j--)
			{
				if (UsableRows[j]->StatToModify == NewMod.ModifiedStat)
					UsableRows.RemoveAt(j);
			}
		}
	}

	// If tries went over that cap, return the function as false success
	Success = (NewModifiers.Num() >= AmountOfMods);
	return NewModifiers;
}

FWeaponModifier AEquipmentManager::FindWeaponMod(EWeaponTypes WeaponType, TArray<FWeaponModifierRow*> UsableRows, bool& Success)
{
	FWeaponModifier NewMod;
	int32 RandomIndex;

	if (UsableRows.Num() <= 0)
	{
		Success = false;
		return NewMod;
	}

	RandomIndex = FMath::RandRange(0, UsableRows.Num() - 1);

	// Which stat it is that is supposed to be modified
	NewMod.ModifiedStat = UsableRows[RandomIndex]->StatToModify;

	// Get a random value between the low and high and round it to nearest integer
	NewMod.ModifierValue = FMath::RoundToInt(FMath::RandRange(UsableRows[RandomIndex]->RangeLow, UsableRows[RandomIndex]->RangeHigh));

	// Create the display string
	FString DisplayString;
	DisplayString = (NewMod.ModifierValue > 0.0f) ? FString::SanitizeFloat(FMath::Abs(NewMod.ModifierValue)) + "%" + " Increased " : FString::SanitizeFloat(FMath::Abs(NewMod.ModifierValue)) + "%" + " Decreased ";
	DisplayString += (WeaponStatAsString(NewMod.ModifiedStat));
	NewMod.ModifierDisplayString = DisplayString;

	Success = true;
	return NewMod;
}

TArray<FWeaponModifier> AEquipmentManager::GetWeaponPreSetMods(FWeaponBases WeaponBaseRow)
{
	TArray<FWeaponModifier> PreSetModifiers;

	for (int i = 0; i < WeaponModifierRowNames.Num(); i++)
	{
		// If there is a match between the selected row from the base weapon table's pre-set mods and one from the mod data table
		if (WeaponBaseRow.PreSetMods.Contains(WeaponModifierRowNames[i].ToString()))
		{
			FWeaponModifier NewMod;
			FString ContextString;
			FWeaponModifierRow* ModRow = ModifiersDataTable->FindRow<FWeaponModifierRow>(WeaponModifierRowNames[i], ContextString);

			if (ModRow)
			{
				// Which stat it is that is supposed to be modified
				NewMod.ModifiedStat = ModRow->StatToModify;

				// Get a random value between the low and high and round it to nearest integer
				NewMod.ModifierValue = FMath::RoundToInt(FMath::RandRange(ModRow->RangeLow, ModRow->RangeHigh));

				// Create the display string
				FString DisplayString;
				DisplayString = (NewMod.ModifierValue > 0.0f) ? FString::SanitizeFloat(FMath::Abs(NewMod.ModifierValue)) + "%" + " Increased " : FString::SanitizeFloat(FMath::Abs(NewMod.ModifierValue)) + "%" + " Decreased ";
				DisplayString += (WeaponStatAsString(NewMod.ModifiedStat));
				NewMod.ModifierDisplayString = DisplayString;

				// Add the mod to the array
				PreSetModifiers.Add(NewMod);
			}
		}
	}

	return PreSetModifiers;
}

#pragma endregion

#pragma region Stats, Scaling and Base

FWeaponAttributes AEquipmentManager::ScaleWeaponStats(FWeaponAttributes BaseStats, TArray<FWeaponModifier> Modifiers, ERarity NewRarity, EDamageTypes NewDamageType)
{
	FWeaponAttributes NewScaledStats;

	// Set all the scaled stats to the base values, incase there is no scaling for that particular stat
	NewScaledStats = BaseStats;
	NewScaledStats.Rarity = NewRarity;
	NewScaledStats.DamageType = NewDamageType;

	// Loop through all the mods and check which stats are modified
	// Different stats are handled differently
	// Some of the values are made sure they only have 2 decimals using string conversion and formatting
	float Multiplier = 0;
	FString StringVal = "";
	for (auto Mod(Modifiers.CreateIterator()); Mod; Mod++)
	{
		// For mods that use a multipler value. In the data table percentage mods are listed as 20, 30, -10 etc.
		// So calculate the mupltier here for those values.
		Multiplier = (((*Mod).ModifierValue + 100.0f) / 100.0f);

		switch ((*Mod).ModifiedStat)
		{
		case EWeaponStats::WS_IncreasedDamage:
			NewScaledStats.DamageLow = FMath::RoundToInt((BaseStats.DamageLow * Multiplier));
			NewScaledStats.DamageHigh = FMath::RoundToInt((BaseStats.DamageHigh * Multiplier));
			break;

		case EWeaponStats::WS_CritChance:
			StringVal = UShooterStaticLibrary::GetFloatAsStringWithPrecision(BaseStats.CritChance * Multiplier, 2);
			NewScaledStats.CritChance = FCString::Atof(*StringVal);
			break;

		case EWeaponStats::WS_CritDamage:
			StringVal = UShooterStaticLibrary::GetFloatAsStringWithPrecision(BaseStats.CritDamage * Multiplier, 2);
			NewScaledStats.CritDamage = FCString::Atof(*StringVal);
			break;

			// Reload speed is a bit different, faster reload speed is a lower ReloadSpeed value
		case EWeaponStats::WS_ReloadSpeed:
			StringVal = UShooterStaticLibrary::GetFloatAsStringWithPrecision((BaseStats.ReloadSpeed * Multiplier) - BaseStats.ReloadSpeed, 2);
			NewScaledStats.ReloadSpeed -= FCString::Atof(*StringVal);
			break;

		case EWeaponStats::WS_FireRate:
			StringVal = UShooterStaticLibrary::GetFloatAsStringWithPrecision(BaseStats.FireRate * Multiplier, 2);
			NewScaledStats.FireRate = FCString::Atof(*StringVal);
			break;

		case EWeaponStats::WS_ProjectileSpeed:
			NewScaledStats.ProjectileSpeed = (BaseStats.ProjectileSpeed * Multiplier);
			break;

			// Magazine is an int, so round it to nearest int value
		case EWeaponStats::WS_MagazineSize:
			NewScaledStats.MagazineSize = FMath::RoundToInt((BaseStats.MagazineSize * Multiplier));
			break;

		case EWeaponStats::WS_ExplosionRadius:
			StringVal = UShooterStaticLibrary::GetFloatAsStringWithPrecision(BaseStats.ExplosionRadius * Multiplier, 2);
			NewScaledStats.ExplosionRadius = FCString::Atof(*StringVal);
			break;
		}
	}

	return NewScaledStats;
}

FWeaponAttributes AEquipmentManager::GetWeaponBaseStats(FName EquipmentName)
{
	FWeaponAttributes NewAttributes;

	if (BaseWeaponsDataTable == nullptr)
		return NewAttributes;

	// Find the row that contains this weapons name
	FWeaponBases* FoundRow = nullptr;
	FString ContextString;
	for (int i = 0; i < WeaponBaseRowNames.Num(); i++)
	{
		if (WeaponBaseRowNames[i].ToString().Contains(EquipmentName.ToString()))
		{
			// Found a matching name, try getting the row
			FoundRow = BaseWeaponsDataTable->FindRow<FWeaponBases>(WeaponBaseRowNames[i], ContextString);
			break;
		}
	}

	if (FoundRow)
	{
		// If the row was found, set all the base stats to the row stats.
		NewAttributes.Rarity = FoundRow->Rarity;
		NewAttributes.DamageType = FoundRow->DamageType;
		NewAttributes.DamageLow = FoundRow->DamageLow;
		NewAttributes.DamageHigh = FoundRow->DamageHigh;
		NewAttributes.CritChance = FoundRow->CritChance;
		NewAttributes.CritDamage = FoundRow->CritDamage;
		NewAttributes.ReloadSpeed = FoundRow->ReloadSpeed;
		NewAttributes.FireRate = FoundRow->FireRate;
		NewAttributes.ProjectileSpeed = FoundRow->ProjectileSpeed;
		NewAttributes.MagazineSize = FoundRow->MagazineSize;
		NewAttributes.ProjectileAmount = FoundRow->ProjectileAmount;
		NewAttributes.ExplosionRadius = FoundRow->ExplosionRadius;
	}

	return NewAttributes;
}

#pragma endregion

#pragma region Helper Functions

FWeaponBases* AEquipmentManager::GetRandomRow(int32& Index)
{
	int32 Tries = 0;
	bool FoundRow = false;
	FWeaponBases* TempRow = nullptr;
	FString ContextString;
	int32 TempIndex = 0;

	// Find a weapon base class that can be randomly selected
	while (!FoundRow)
	{
		// Stop it from looping forever
		Tries++;
		if (Tries > 100)
		{
			// Make sure it's empty if the while loop breaks out
			TempRow = nullptr;
			break;
		}

		// Get a random row name from the array with the different base weapon rows from the data table
		TempIndex = FMath::RandRange(0, WeaponBaseRowNames.Num() - 1);

		// Get the row from the randomized value, which selects the random base weapon.
		TempRow = BaseWeaponsDataTable->FindRow<FWeaponBases>(WeaponBaseRowNames[TempIndex], ContextString);

		// Stop the loop if the row is valid and cannot be randomly selected
		if (TempRow && TempRow->CanBeRandomlySelected)
		{
			FoundRow = true;
		}
	}

	Index = TempIndex;
	return TempRow;
}

FWeaponBases* AEquipmentManager::GetRandomRowFromType(EWeaponTypes Type, int32& Index)
{
	// Find all the rom names that has the matching weapon type
	TArray<FName> TrimmedRowNames;
	for (int i = 0; i < WeaponBaseRowNames.Num(); i++)
	{
		if (WeaponBaseRowNames[i].ToString().Contains(ShortenedWeaponType(Type)))
		{
			TrimmedRowNames.Add(WeaponBaseRowNames[i]);
		}
	}

	// Make sure there is elements to look through
	if (TrimmedRowNames.Num() <= 0)
		return nullptr;

	int32 Tries = 0;
	bool FoundRow = false;
	FWeaponBases* TempRow = nullptr;
	FString ContextString;
	int32 TempIndex = 0;
	while (!FoundRow)
	{
		// Stop it from looping forever
		Tries++;
		if (Tries > 100)
		{
			// Make sure it's empty if the while loop breaks out
			TempRow = nullptr;
			break;
		}

		// Select a random int within the range and then get the row from the data table.
		int32 RandomIndex = FMath::RandRange(0, TrimmedRowNames.Num() - 1);
		TempRow = BaseWeaponsDataTable->FindRow<FWeaponBases>(TrimmedRowNames[RandomIndex], ContextString);

		if (TempRow && TempRow->CanBeRandomlySelected)
		{
			WeaponBaseRowNames.Find(TrimmedRowNames[RandomIndex], Index);
			FoundRow = true;
		}
	}

	return TempRow;
}


FString AEquipmentManager::ShortenedWeaponType(EWeaponTypes WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponTypes::WT_AssaultRifle:
		return "AR";
		break;

	case EWeaponTypes::WT_SubmachineGun:
		return "SMG";
		break;

	case EWeaponTypes::WT_RocketLauncher:
		return "RL";
		break;

	case EWeaponTypes::WT_GrenadeLauncher:
		return "GL";
		break;
	};

	return "";
}


EWeaponTypes AEquipmentManager::ShortenedWeaponTypeBack(FString ShortenedType)
{
	if (ShortenedType == "AR")
		return EWeaponTypes::WT_AssaultRifle;
	else if (ShortenedType == "SMG")
		return EWeaponTypes::WT_SubmachineGun;
	else if (ShortenedType == "RL")
		return EWeaponTypes::WT_RocketLauncher;
	else if (ShortenedType == "GL")
		return EWeaponTypes::WT_GrenadeLauncher;

	return EWeaponTypes::WT_None;
}


FString AEquipmentManager::WeaponStatAsString(EWeaponStats WeaponStat)
{
	switch (WeaponStat)
	{
	case EWeaponStats::WS_IncreasedDamage:
		return "Damage";
		break;

	case EWeaponStats::WS_CritChance:
		return "Crit Chance";
		break;

	case EWeaponStats::WS_CritDamage:
		return "Crit Damage";
		break;

	case EWeaponStats::WS_ReloadSpeed:
		return "Reload Speed";
		break;

	case EWeaponStats::WS_FireRate:
		return "Fire Rate";
		break;

	case EWeaponStats::WS_ProjectileSpeed:
		return "Proj Speed";
		break;

	case EWeaponStats::WS_MagazineSize:
		return "Magazine Size";
		break;

	case EWeaponStats::WS_ExplosionRadius:
		return "Explosion Radius";
		break;

	default:
		return "";
		break;
	};

}

#pragma endregion