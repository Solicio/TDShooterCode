// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#ifndef __TDSHOOTER_H__
#define __TDSHOOTER_H__

#include "Engine/Engine.h"
#include "UnrealNetwork.h"
#include "Online.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

FORCEINLINE void PrintText(FString Text, int32 Key = -1, float Duration = 2.0f, FColor Color = FColor::Green)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Text);
	}
}

template <typename ASpawnBP>
FORCEINLINE ASpawnBP* SpawnBP(
	UWorld* TheWorld,
	UClass* TheBP,
	const FVector& Loc,
	const FRotator& Rot,
	const ESpawnActorCollisionHandlingMethod bNoCollisionFail = ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
	AActor* Owner = NULL,
	APawn* Instigator = NULL
	)
{
	if (!TheWorld) return NULL;
	if (!TheBP) return NULL;
	//~~~~~~~~~~~

	FActorSpawnParameters SpawnInfo;
	/*SpawnInfo.bNoCollisionFail = bNoCollisionFail;*/
	SpawnInfo.SpawnCollisionHandlingOverride = bNoCollisionFail;
	SpawnInfo.Owner = Owner;
	SpawnInfo.Instigator = Instigator;
	SpawnInfo.bDeferConstruction = false;

	return TheWorld->SpawnActor<ASpawnBP>(TheBP, Loc, Rot, SpawnInfo);
}

template<typename TEnum>
static FORCEINLINE FString GetEnumValueToString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetEnumName((int32)Value);
}

#endif
