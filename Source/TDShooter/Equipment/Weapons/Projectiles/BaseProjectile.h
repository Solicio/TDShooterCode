// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Global/ShooterStaticLibrary.h"

#include "Equipment/Weapons/Explosion.h"

#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class AShooterCharacterBase;

UCLASS()
class TDSHOOTER_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
	public:	
	
		ABaseProjectile();

		/** The root collision component */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		USphereComponent* SphereCollisionComp;

		/** The projectile mesh */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		UStaticMeshComponent* ProjMesh;

		/** The current faction this character has */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		UProjectileMovementComponent* ProjectileMovement;


		/** The current faction this projectile has, cannot hit of same faction. Can hit all other factions */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base")
		EFactions Faction;
		
		/** The character that spawned this projectile */
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base")
		AShooterCharacterBase* InstigatorCharacter;

		/** The color of the projectiles and effects */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		FLinearColor DamageTypeColor;

		/** How long the projectile should stay alive before destroying itself. Incase it never hits anything */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		float LifeTime;

		/** If this projectile should handle it's movement on server and replicate down to clients
		*	Or if the clients should handle movement themselves.
		*	Should be used in cases of projectile changing direction. I.E Grenade launcher.
		*/
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base")
		bool MovementOnServer;

		/** If the projectile should be destroyed on hit  */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		bool DestroyOnHit;

		/** If the projectile should deal any damage when the projectile hits a target, or if it is just the explosion that deals damage */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		bool IsExplosionProjectile;

		/** The explosion class that is spawned when the projectile explodes */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		TSubclassOf<AExplosion> ExplosionClass;

		/** Requires this projectile to be a replicated version on a client. When this var is true, hide the projectile from the client.
		*	Will be hidden until server version is destroyed.
		*/
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		bool ClientShouldHide;
		

		/** Which type of damage the weapon has. Different types have different effects. (Set by weapon on spawn) */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		EDamageTypes DamageType;

		/** The damage range of this weapon, can roll value between Low and High when dealing damage. (Set by weapon on spawn) */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageLow;

		/** The damage range of this weapon, can roll value between Low and High when dealing damage. (Set by weapon on spawn) */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageHigh;

		/** The crit chance of the weapon, a crit hit is increased by the crit damage variable. (Set by weapon on spawn) */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float CritChance;

		/** How much the crits should increase the damage done. This value is a multiplier, 20% crit damage = 1.2f. (Set by weapon on spawn) */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float CritDamage;

		/** The size of the sphere collision used for overlap checking. Only relevant for explosion projectiles */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
		float ExplosionRadius;


		/** Blueprint event that is called after the projectile has hit something */
		UFUNCTION(BlueprintImplementableEvent, Category = "Base")
		void BPProjectileHit(AActor* HitActor, FVector Location);

		/** Function called if the projecile is a explosion projectile on hit */
		virtual void HitExplosion(AActor* HitActor, FVector Location);

		void ProjectileExplode();

		/** Called after spawning the projectile. Used to set the velocity of the projectile, as well as the color of the effects */
		void SetInitialValues(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, FName ProfileName);
		
		/** Sets the initial colors of the particle effects */
		UFUNCTION(BlueprintImplementableEvent, Category = "Base")
		void BPSetColors();
		

	protected:

		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastSetInitialValues(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, bool SetVelocity, FName ProfileName);
		void MulticastSetInitialValues_Implementation(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, bool SetVelocity, FName ProfileName);
		bool MulticastSetInitialValues_Validate(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, bool SetVelocity, FName ProfileName);

		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastProjHit(AActor* OtherActor, FVector Location);
		void MulticastProjHit_Implementation(AActor* OtherActor, FVector Location);
		bool MulticastProjHit_Validate(AActor* OtherActor, FVector Location);

		UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

		//UFUNCTION()
		//void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaSeconds) override;

};
