// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../NPC_Base_Controller.h"
#include "NPC_Fighter_Controller.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API ANPC_Fighter_Controller : public ANPC_Base_Controller
{
	GENERATED_BODY()
	
	public:

		ANPC_Fighter_Controller();

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
	
};
