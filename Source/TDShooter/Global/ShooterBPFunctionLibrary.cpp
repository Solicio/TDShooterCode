// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "ShooterBPFunctionLibrary.h"

//#include "../Characters/Player/ShooterCharacter.h"
//#include "../Characters/NPC/NPC_Base.h"
#include "../Characters/ShooterCharacterBase.h"


EFactions UShooterBPFunctionLibrary::GetCharacterFaction(ACharacter* Character)
{
	if (Character == nullptr)
		return EFactions::F_None;

	if (Cast<AShooterCharacterBase>(Character))
	{
		return Cast<AShooterCharacterBase>(Character)->Faction;
	}

	return EFactions::F_None;
}

bool UShooterBPFunctionLibrary::StoredWeaponEqual(FStoredWeapon FirstItem, FStoredWeapon SecondItem)
{
	return (FirstItem == SecondItem);
}