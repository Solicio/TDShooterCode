// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interaction/InteractableActor.h"
#include "IA_LootContainerBase.generated.h"

/** The different types of loot containers in the game. */
UENUM(BlueprintType)
enum class ELootTypes : uint8
{
	LT_None				UMETA(DisplayName = "None"),
	LT_Weapon			UMETA(DisplayName = "Weapon"),
	LT_HealthPack		UMETA(DisplayName = "HealthPack")
};

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AIA_LootContainerBase : public AInteractableActor
{
	GENERATED_BODY()
	
	
	public:

		AIA_LootContainerBase();

		/** What type of loot is inside this container */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Container Base")
		ELootTypes LootType;


		/** What happens when a character interacts with this actor
		*	Will be MultiCasted down to clients.
		*	Override to implement own logic for interactable actor
		*	Always call Super::InteractLogic() when overriding.
		*/
		UFUNCTION(Category = "Interactable Actor")
		virtual void InteractLogic(AShooterCharacterBase* InteractingCharacter) override;


		/** Function that is called when the InteractedWith variable is changed.
		*	This function should handle things such as chests being opened or doors that has been opened.
		*	The reason for this is if a player is too far away from the actor when the multicast event is fired the player will not receive it.
		*	But if the player then comes back the actor will be out of sync. But using this rep notify for those things. They will stay in sync.
		*
		*	Same with InteractLogic, override and call Super::
		*/
		UFUNCTION(Category = "Interactable Actor")
		virtual void OnRep_InterctedWith() override;


		/** Tell the container to handle it's logic.
		*	Weapon loot containers should add itself to the players inventory
		*	Health packs should increase the players health.
		*	Needs to be overriden in child classes.
		*/
		virtual void HandleContainer(AShooterCharacterBase* InteractingCharacter);


		/** Tells the container that it should set the colors to match it's rarity */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Container Base")
		void SetRarityColors();


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaTime) override;
	
};
