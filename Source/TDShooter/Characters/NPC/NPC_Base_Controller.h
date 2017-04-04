// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NPC_Base.h"

#include "AIController.h"
#include "NPC_Base_Controller.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API ANPC_Base_Controller : public AAIController
{
	GENERATED_BODY()
	
	public:

		ANPC_Base_Controller();

		/** Take control of pawn */
		virtual void Possess(class APawn* InPawn) override;


	protected:

		/** The character this controller possess */
		ANPC_Base* Character;

		/*A Behavior tree component in order to be able to call specific functions like starting our BT*/
		UBehaviorTreeComponent* BehaviorTreeComp;

		/*A Blackboard component which will be used to initialize our Blackboard Values*/
		UBlackboardComponent* BlackboardComp;

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
