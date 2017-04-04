// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "TDShooterGameState.h"



ATDShooterGameState::ATDShooterGameState()
{

}

void ATDShooterGameState::BeginPlay()
{
	Super::BeginPlay();

	SpawnEquipmentManager();

}

void ATDShooterGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDShooterGameState, EquipmentManager);
}

void ATDShooterGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

//AWeaponManager* ATDShooterGameState::GetWeaponManager()
//{
//	return WeaponManager;
//}
//
//void ATDShooterGameState::SpawnWeaponManager()
//{
//	// Create the weapon manager on the server.
//	if (Role == ROLE_Authority)
//	{
//		WeaponManager = SpawnBP<AWeaponManager>(GetWorld(), WeaponManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
//	}
//}

AEquipmentManager* ATDShooterGameState::GetEquipmentManager()
{
	return EquipmentManager;
}

void ATDShooterGameState::SpawnEquipmentManager()
{
	// Create the weapon manager on the server.
	if (Role == ROLE_Authority)
	{
		EquipmentManager = SpawnBP<AEquipmentManager>(GetWorld(), EquipmentManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}
}