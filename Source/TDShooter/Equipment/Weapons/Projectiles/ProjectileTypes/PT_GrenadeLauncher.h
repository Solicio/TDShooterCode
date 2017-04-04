// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../BaseProjectile.h"
#include "PT_GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API APT_GrenadeLauncher : public ABaseProjectile
{
	GENERATED_BODY()
	
	public:

		/** How long after spawning the grenade should explode */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float ExplosionDelay;

		/** How long after spawning the grenade should explode */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float CurrentDelay;

		/** Function called if the projecile is a explosion projectile on hit */
		virtual void HitExplosion(AActor* HitActor, FVector Location) override;


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
