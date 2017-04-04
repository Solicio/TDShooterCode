// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "InventoryItemBase.h"


// Sets default values
AInventoryItemBase::AInventoryItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void AInventoryItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInventoryItemBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInventoryItemBase, ItemName);
	DOREPLIFETIME(AInventoryItemBase, ItemIndex);
	DOREPLIFETIME(AInventoryItemBase, ItemType);
	DOREPLIFETIME(AInventoryItemBase, ItemSlotType);
	DOREPLIFETIME(AInventoryItemBase, ItemIcon);
}

// Called every frame
void AInventoryItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (InventoryItemsPointer != nullptr)
	//{
	//	TArray<AInventoryItemBase*>& TempArray = *InventoryItemsPointer;
	//
	//	for (int i = 0; i < TempArray.Num(); i++)
	//	{
	//		if (TempArray[i])
	//		{
	//			PrintText(TempArray[i]->GetName(), -1, 0.0f, FColor::Green);
	//		}
	//		else
	//		{
	//			//PrintText("Invalid", -1, 0.0f, FColor::Green);
	//		}
	//	}
	//}
	//else if(InventoryItemsPointer == nullptr && Role == ROLE_Authority)
	//{
	//	PrintText("Invalid", -1, 0.0f, FColor::Green);
	//}
}

//void AInventoryItemBase::SetInventoryArray(TArray<AInventoryItemBase*>* ArrayPointer)
//{
//	InventoryItemsPointer = ArrayPointer;
//}