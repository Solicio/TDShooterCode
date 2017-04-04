// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Equipment/EquipmentManager.h"

#include "GameFramework/GameStateBase.h"
#include "TDShooterGameState.generated.h"


/**
 * 
 */
UCLASS()
class TDSHOOTER_API ATDShooterGameState : public AGameStateBase
{
	GENERATED_BODY()

	public:

		ATDShooterGameState();

		//--- Equipment Manager ---//

		//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game State")
		//AWeaponManager* GetWeaponManager();

		//UPROPERTY(EditDefaultsOnly, Category = "Game State")
		//TSubclassOf<class AWeaponManager> WeaponManagerClass;

		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game State")
		AEquipmentManager* GetEquipmentManager();

		UPROPERTY(EditDefaultsOnly, Category = "Game State")
		TSubclassOf<class AEquipmentManager> EquipmentManagerClass;


	protected:

		/** Reference to the created equipment manager. Used to generate/create weapons and other equipments */
		UPROPERTY(Replicated)
		AEquipmentManager* EquipmentManager;

		void SpawnEquipmentManager();

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
