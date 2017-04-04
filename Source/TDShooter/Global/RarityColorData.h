// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "RarityColorData.generated.h"

USTRUCT(BlueprintType)
struct FRarityColors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Color")
	FLinearColor Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Color")
	FLinearColor Uncommon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Color")
	FLinearColor Rare;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Color")
	FLinearColor Epic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity Color")
	FLinearColor Legendary;

};

/**
 * 
 */
UCLASS(BlueprintType)
class TDSHOOTER_API URarityColorData : public UDataAsset
{
	GENERATED_BODY()
	
	public:

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRarityColors Colors;
	
	
};
