// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../BaseProjectile.h"
#include "PT_RocketLauncher.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API APT_RocketLauncher : public ABaseProjectile
{
	GENERATED_BODY()
	
	public:

		/** Function called if the projecile is a explosion projectile on hit */
		virtual void HitExplosion(AActor* HitActor, FVector Location) override;


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

};
