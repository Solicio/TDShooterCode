// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../NPC_Base.h"
#include "NPC_Fighter.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API ANPC_Fighter : public ANPC_Base
{
	GENERATED_BODY()
	
	public:

		ANPC_Fighter();

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
	
};
