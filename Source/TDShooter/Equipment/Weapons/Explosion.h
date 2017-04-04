// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Global/ShooterStaticLibrary.h"

#include "GameFramework/Actor.h"
#include "Explosion.generated.h"

UCLASS()
class TDSHOOTER_API AExplosion : public AActor
{
	GENERATED_BODY()
	
	public:	

		// Sets default values for this actor's properties
		AExplosion();

		/** Root component to hold all the other components */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
		USceneComponent* SceneRoot;

		/** The collision component used to find characters to hit */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
		USphereComponent* SphereCollisionComp;

		/** The light that lights up when the explosion goes off */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
		UPointLightComponent* ExplosionLightComp;
		

		/** The system that should be created when exploded */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
		UParticleSystem* ExplosionParticleSystem;

		/** The light actor that provides light to the explosion */
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
		//UClass* LightActor;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
		USoundCue* ExplosionSound;


		/** The current faction this projectile has, cannot hit of same faction. Can hit all other factions */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Explosion")
		EFactions Faction;

		/** The character that spawned this projectile */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Explosion")
		ACharacter* InstigatorCharacter;

		/** The size of the sphere collision used for overlap checking. */
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
		//float ExplosionRadius;

		/** If there should be a line of check trace so that character cannot be hit through walls. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
		bool CheckLineOfSight;

		/** How long the explosion will stay alive until it's deleted. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
		float DestroyDelayMax;


		// Which type of damage the weapon has. Different types have different effects. (Set by projectile on spawn)
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		EDamageTypes DamageType;

		// The damage range of this weapon, can roll value between Low and High when dealing damage. (Set by projectile on spawn)
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageLow;

		// The damage range of this weapon, can roll value between Low and High when dealing damage. (Set by projectile on spawn)
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageHigh;

		// The crit chance of the weapon, a crit hit is increased by the crit damage variable. (Set by projectile on spawn)
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float CritChance;

		// How much the crits should increase the damage done. This value is a multiplier, 20% crit damage = 1.2f. (Set by projectile on spawn)
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float CritDamage;


		/** Sets all the damage stats of the explosion */
		UFUNCTION(BlueprintCallable, Category = "Explosion")
		void SetDamageStats(EDamageTypes Type, float DmgLow, float DmgHigh, float CritC, float CritD);

		/** Function that checks the area for actors to hit and deal damage too. */
		UFUNCTION(BlueprintCallable, Category = "Explosion")
		void ExplodeArea(float ExplosionRad);

		/** Spawns the particle system and plays the explosion sound */
		UFUNCTION(BlueprintCallable, Category = "Explosion")
		void ExplodeEffect(FLinearColor EffectColor, float ExplosionRad);


	protected:

		float DestroyDelay;

		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastExplodeAreaEffect(FLinearColor EffectColor);
		void MulticastExplodeAreaEffect_Implementation(FLinearColor EffectColor);
		bool MulticastExplodeAreaEffect_Validate(FLinearColor EffectColor);

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
