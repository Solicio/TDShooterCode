// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../NPC_Base.h"
#include "NPC_NonCombat.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API ANPC_NonCombat : public ANPC_Base
{
	GENERATED_BODY()
	
	public:

		ANPC_NonCombat();

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
