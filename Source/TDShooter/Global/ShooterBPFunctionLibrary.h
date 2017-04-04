// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ShooterStaticLibrary.h"
#include "Equipment/Weapons/BaseWeapon.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShooterBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UShooterBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	public:

		UFUNCTION(BlueprintCallable, Category = "ShooterBPLibrary")
		static EFactions GetCharacterFaction(ACharacter* Character);

		UFUNCTION(BlueprintCallable, Category = "ShooterBPLibrary")
		static bool StoredWeaponEqual(FStoredWeapon FirstItem, FStoredWeapon SecondItem);
	
};
