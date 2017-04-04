// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "NPC_Base_Controller.h"


// Sets default values
ANPC_Base_Controller::ANPC_Base_Controller()
{
	//Initialize our components
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

// Called when the game starts or when spawned
void ANPC_Base_Controller::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ANPC_Base_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ANPC_Base_Controller::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	//If our character is valid and has a valid Behavior Tree,
	//Initialize the values of the Blackboard and start the tree
	Character = Cast<ANPC_Base>(InPawn);
	if (Character && Character->BehaviorTree->BlackboardAsset)
	{
		//Initialize the blackboard values
		BlackboardComp->InitializeBlackboard(*Character->BehaviorTree->BlackboardAsset);

		//Start the tree
		BehaviorTreeComp->StartTree(*Character->BehaviorTree);

	}
}



