// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "InteractableActor.h"


// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// Create component used for line trace to see if mouse is over
	TraceCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trace Collision Box"));
	TraceCollisionBox->SetBoxExtent(FVector(25.0f, 25.0f, 25.0f));
	TraceCollisionBox->ComponentTags.Add("InteractableActor");
	TraceCollisionBox->bGenerateOverlapEvents = false;
	TraceCollisionBox->SetCollisionProfileName("InteractTraceBox");
	TraceCollisionBox->SetupAttachment(Mesh);

	// Create component used for ranged based triggers
	CharacterOverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Character Overlap Box"));
	CharacterOverlapSphere->SetSphereRadius(10.0f);
	CharacterOverlapSphere->bGenerateOverlapEvents = true;
	CharacterOverlapSphere->SetCollisionProfileName("OverlapAll");
	CharacterOverlapSphere->SetupAttachment(Mesh);

	Name = "Interactable Actor";
	Active = true;
	InteractedWith = false;
	InfiniteInteractions = false;
	InteractRange = 200.0f;

	bReplicates = true;

	PlayerOnlyTrigger = true;

	Tags.Add("Interactable");

	CharacterOverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnOverlapBegin);
	CharacterOverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnOverlapEnd);
}

void AInteractableActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableActor, Active);
	DOREPLIFETIME(AInteractableActor, InteractedWith);
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	// Set the radius of the overlap sphere to interaction range if the interaction can be done through range
	if (RangeTriggeredInteraction)
	{
		CharacterOverlapSphere->SetSphereRadius(InteractRange);

		// DEBUG TEST
		CharacterOverlapSphere->bHiddenInGame = false;
	}
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableActor::Interact(AShooterCharacterBase* InteractingCharacter)
{
	if (Role < ROLE_Authority)
		return;

	// Run the interact logic on the server and Multicast this call down to all the clients 
	MulticastInteract(InteractingCharacter);
}

void AInteractableActor::MulticastInteract_Implementation(AShooterCharacterBase* InteractingCharacter)
{
	// Run the logic on the server and the clients
	InteractLogic(InteractingCharacter);
}

bool AInteractableActor::MulticastInteract_Validate(AShooterCharacterBase* InteractingCharacter)
{
	return true;
}

void AInteractableActor::InteractLogic(AShooterCharacterBase* InteractingCharacter)
{
	// Needs to be overriden and implement logic
	// Always call Super::InteractLogic();

	//PrintText("Interact Logic Base Func", -1, 10.0f);

	// Set so that the actor has been interacted with.
	if(!InfiniteInteractions)
		InteractedWith = true;
	
	OnRep_InterctedWith();

	// Call potential blueprint function
	InteractBPEvent(InteractingCharacter);
}

void AInteractableActor::OnRep_InterctedWith()
{
	//PrintText("Interact Logic On Rep Interact - Base", -1, 10.0f);

	// Call potential blueprint function
	OnRepInteractedWithBPEvent();
}

void AInteractableActor::ResetInteractActor()
{
	InteractedWith = false;
	OnRep_InterctedWith();
}

void AInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role < ROLE_Authority || !RangeTriggeredInteraction)
		return;

	if (OtherActor != nullptr && OtherActor->ActorHasTag("ShooterCharacter"))
	{
		// If only the player can trigger the interaction based on range, return if it's not player
		if (PlayerOnlyTrigger && !OtherActor->ActorHasTag("Player"))
			return;

		// interact with the actor
		Interact(Cast<AShooterCharacterBase>(OtherActor));
	}
}

void AInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role < ROLE_Authority || !RangeTriggeredInteraction)
		return;

	if (OtherActor != nullptr)
	{

	}
}