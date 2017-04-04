// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "IA_Chest.h"

AIA_Chest::AIA_Chest()
{
	
}

// Called when the game starts or when spawned
void AIA_Chest::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AIA_Chest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIA_Chest::InteractLogic(AShooterCharacterBase* InteractingCharacter)
{
	// Needs to be overriden and implement logic
	// Always call Super::InteractLogic();

	//PrintText("Interact Logic Chest Func", -1, 10.0f);

	// Call the super function for the BP event and interacted with variable set
	Super::InteractLogic(InteractingCharacter);
}

void AIA_Chest::OnRep_InterctedWith()
{
	//PrintText("Interact Logic On Rep Interact - Chest", -1, 10.0f);
	Super::OnRep_InterctedWith();
}