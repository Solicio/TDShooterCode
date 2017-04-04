// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/ShooterCharacterBase.h"

#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS()
class TDSHOOTER_API AInteractableActor : public AActor
{
	GENERATED_BODY()
	
	public:	
	
		AInteractableActor();

		/** Root component to hold all the other components */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable Actor")
		UStaticMeshComponent* Mesh;

		/** Cube component for trace detecting */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable Actor")
		UBoxComponent* TraceCollisionBox;

		/** Sphere component for checking if a character has entered the range */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable Actor")
		USphereComponent* CharacterOverlapSphere;


		/** Name of the actor */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Actor")
		FString Name;

		/** This is the max range characters can interact with this actor from, if RangeTriggeredInteraction is true, this range will be used */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Actor")
		float InteractRange;

		/** If the object is active, if the object can be interacted with  */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Interactable Actor")
		bool Active;

		/** If the actor has been interacted with */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InterctedWith, Category = "Interactable Actor")
		bool InteractedWith;

		/** If the object never should set interacted with, so that it can be interacted with forever, for like the stash */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Interactable Actor")
		bool InfiniteInteractions;

		/** If this actor interaction is triggered when a character walks into range of the actor, range = InteractRange */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Actor")
		bool RangeTriggeredInteraction;

		/** If only the player can trigger the ranged base interaction */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Actor", meta = (EditCondition = "RangeTriggeredInteraction"))
		bool PlayerOnlyTrigger;


		// ----- Functions ----- //

		/** Call this function to interact with this actor. 
		*	Must be called on server. 
		*	This function makes sure that the clients also execute the interact logic for this actor.
		*/
		UFUNCTION(BlueprintCallable, Category = "Interactable Actor")
		void Interact(AShooterCharacterBase* InteractingCharacter);


		/** What happens when a character interacts with this actor
		*	Will be MultiCasted down to clients.
		*	Override to implement own logic for interactable actor
		*	Always call Super::InteractLogic() when overriding.
		*/
		UFUNCTION(Category = "Interactable Actor")
		virtual void InteractLogic(AShooterCharacterBase* InteractingCharacter);

		/** Function that is called when the InteractedWith variable is changed.
		*	This function should handle things such as chests being opened or doors that has been opened.
		*	The reason for this is if a player is too far away from the actor when the multicast event is fired the player will not receive it.
		*	But if the player then comes back the actor will be out of sync. But using this rep notify for those things. They will stay in sync.
		*
		*	Same with InteractLogic, override and call Super::
		*/
		UFUNCTION(Category = "Interactable Actor")
		virtual void OnRep_InterctedWith();


		/** This event is called by the InteractLogic() function, can be used for blueprint events */
		UFUNCTION(BlueprintImplementableEvent, Category = "Interactable Actor")
		void InteractBPEvent(AShooterCharacterBase* InteractingCharacter);

		/** This event is called by the OnRep_InteractedWith() function, can be used for blueprint events */
		UFUNCTION(BlueprintImplementableEvent, Category = "Interactable Actor")
		void OnRepInteractedWithBPEvent();

		/** Reset the actor so that it can be interacted with again */
		UFUNCTION(BlueprintCallable, Category = "Interactable Actor")
		void ResetInteractActor();


	protected:

		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastInteract(AShooterCharacterBase* InteractingCharacter);
		void MulticastInteract_Implementation(AShooterCharacterBase* InteractingCharacter);
		bool MulticastInteract_Validate(AShooterCharacterBase* InteractingCharacter);

		UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

};
