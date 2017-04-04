// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ShooterCharacterBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Components/WidgetComponent.h"
#include "HUD/NPC/NPC_StatDisplay.h"

#include "GameFramework/Character.h"
#include "NPC_Base.generated.h"


UCLASS()
class TDSHOOTER_API ANPC_Base : public AShooterCharacterBase
{
	GENERATED_BODY()

	public:

		ANPC_Base();

		/*A Behavior Tree reference*/
		UPROPERTY(EditDefaultsOnly, Category = "NPC Base")
		UBehaviorTree* BehaviorTree;

		/** This widget will be positioned above the NPC and will display things such as a health bar */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Base")
		UWidgetComponent* StatDisplay;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NPC Base")
		TSubclassOf<class UNPC_StatDisplay> StatDisplayWidgetClass;

		/** The location the stat display will be, relative to the character */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Base")
		FVector RelativeStatDisplayLoc;

		/** The location the stat display will be, relative to the character */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Base")
		FVector2D StatDisplayDrawSize;


		//--- Shop ---//

		/** Array containing all the shop inventory items */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Shop")
		TArray<AInventoryItemBase*> ShopInventory;


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
	
};
