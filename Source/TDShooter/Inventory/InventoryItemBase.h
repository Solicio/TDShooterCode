// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Global/ShooterStaticLibrary.h"

#include "GameFramework/Actor.h"
#include "InventoryItemBase.generated.h"

/** All the different item types there are */
UENUM(BlueprintType)
enum class EItemTypes : uint8
{
	IT_None				UMETA(DisplayName = "None"),
	IT_Weapon			UMETA(DisplayName = "Weapon"),
};


UCLASS()
class TDSHOOTER_API AInventoryItemBase : public AActor
{
	GENERATED_BODY()
	
	public:	

		AInventoryItemBase();

		/** The name of the item. In cases of equipment, this name will be the equipment name */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Base")
		FName ItemName;

		/** The index this item has in the inventory array, also used for equipped items */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Base")
		int32 ItemIndex;

		/** What type of item it is. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Base")
		EItemTypes ItemType;

		/** What type of slot this item represents, inventory, stash, shop, equipped item */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Base")
		EInventorySlotType ItemSlotType;

		/** The icon that should be used for the inventory slot */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Base")
		UTexture2D* ItemIcon;


		///** Setter for the array pointer that points to the array that this item is a part of. */
		//void SetInventoryArray(TArray<AInventoryItemBase*>* ArrayPointer);


		///** Pointer to the array that this item is a part of.
		//*	Required for when the player wants to move around the items.
		//*	Not valid on clients.
		//*/
		//TArray<AInventoryItemBase*>* InventoryItemsPointer;


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaTime) override;	
};
