// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "NPC_Base.h"


// Sets default values
ANPC_Base::ANPC_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BehaviorTree = CreateDefaultSubobject<UBehaviorTree>(TEXT("BehaviorTreeReference"));

	StatDisplay = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatDisplay"));
	StatDisplay->SetupAttachment(RootComponent);

	RelativeStatDisplayLoc = FVector(0.0f, 0.0f, 130.0f);
	StatDisplayDrawSize = FVector2D(110.0f, 50.0f);
	
	GetMesh()->SetCollisionProfileName("Ragdoll");

	InfiniteAmmo = true;
}

void ANPC_Base::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANPC_Base, ShopInventory);
}

// Called when the game starts or when spawned
void ANPC_Base::BeginPlay()
{
	Super::BeginPlay();
	
	if (StatDisplay)
	{
		StatDisplay->SetRelativeLocation(RelativeStatDisplayLoc);
		StatDisplay->SetDrawSize(StatDisplayDrawSize);
		StatDisplay->SetWidgetClass(StatDisplayWidgetClass);
		StatDisplay->SetWidgetSpace(EWidgetSpace::Screen);
		StatDisplay->InitWidget();

		if (StatDisplay->GetUserWidgetObject())
		{
			UNPC_StatDisplay* Widget = Cast<UNPC_StatDisplay>(StatDisplay->GetUserWidgetObject());
			Widget->OwningCharacter = this;
		}
	}
}

// Called every frame
void ANPC_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//// Check the ignore array and remove any empty slots. - 1; i > 0; i--
	//for (int32 i = GetCapsuleComponent()->MoveIgnoreActors.Num() - 1; i >= 0; --i)
	//{
	//	if (GetCapsuleComponent()->MoveIgnoreActors[i] == nullptr || GetCapsuleComponent()->MoveIgnoreActors[i]->IsPendingKill())
	//	{
	//		GetCapsuleComponent()->MoveIgnoreActors.RemoveAt(i);
	//	}
	//}
}


