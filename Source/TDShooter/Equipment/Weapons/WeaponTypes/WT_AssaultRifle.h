// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Equipment/Weapons/BaseWeapon.h"
#include "WT_AssaultRifle.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AWT_AssaultRifle : public ABaseWeapon
{
	GENERATED_BODY()

	public:

		AWT_AssaultRifle();
	
		/** Assualt Rifle weapon fire. */
		UFUNCTION(BlueprintCallable, Category = "AssaultRifle")
		virtual bool FireWeapon(AActor* InstigatorActor, EFactions Faction) override;


	protected:

		/** Function that spawns the projectiles. Called from FireWeapon()
		*	Also sets the projectiles faction, collision profile and damage stats
		*/
		bool SpawnARProjectile(int32 InProjectileAmount, float InProjectileSpeed, UClass* ProjectileBP, AActor* IgnoreActor, EFactions Faction);

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
