// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "DamageColorData.generated.h"

USTRUCT(BlueprintType)
struct FDamageTypeColors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FLinearColor Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FLinearColor Poison;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FLinearColor Ice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FLinearColor Lightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FLinearColor Arcane;

};


/**
 * 
 */
UCLASS(BlueprintType)
class TDSHOOTER_API UDamageColorData : public UDataAsset
{
	GENERATED_BODY()

	public:
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDamageTypeColors Colors;
	
};
