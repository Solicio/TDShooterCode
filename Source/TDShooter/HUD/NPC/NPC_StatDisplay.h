// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "NPC_StatDisplay.generated.h"

class ANPC_Base;

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UNPC_StatDisplay : public UUserWidget
{
	GENERATED_BODY()
	
	public:
		
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat Display")
		ANPC_Base* OwningCharacter;

	
};
