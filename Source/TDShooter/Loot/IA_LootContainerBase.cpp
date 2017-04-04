// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "IA_LootContainerBase.h"


AIA_LootContainerBase::AIA_LootContainerBase()
{

	Tags.Add("LootContainer");
}

// Called when the game starts or when spawned
void AIA_LootContainerBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AIA_LootContainerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIA_LootContainerBase::InteractLogic(AShooterCharacterBase* InteractingCharacter)
{
	// Needs to be overriden and implement logic
	// Always call Super::InteractLogic();

	// Call the super function for the BP event and interacted with variable set
	Super::InteractLogic(InteractingCharacter);

	// If this is on the server, handle the container
	if (Role == ROLE_Authority)
		HandleContainer(InteractingCharacter);
}

void AIA_LootContainerBase::OnRep_InterctedWith()
{
	Super::OnRep_InterctedWith();
}

void AIA_LootContainerBase::HandleContainer(AShooterCharacterBase* InteractingCharacter)
{

}