// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interaction/InteractableActor.h"
#include "IA_Chest.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AIA_Chest : public AInteractableActor
{
	GENERATED_BODY()
	
	public:	
	
		AIA_Chest();

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

	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaTime) override;
	
};
