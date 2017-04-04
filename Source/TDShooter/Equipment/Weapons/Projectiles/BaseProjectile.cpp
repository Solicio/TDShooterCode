// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "BaseProjectile.h"

#include "Characters/ShooterCharacterBase.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComp"));
	RootComponent = SphereCollisionComp;
	//SphereCollisionComp->SetCollisionProfileName("ProjectilePreset");
	SphereCollisionComp->SetCanEverAffectNavigation(false);

	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjMesh"));
	ProjMesh->SetupAttachment(RootComponent);
	ProjMesh->SetCollisionProfileName("NoCollision");

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	
	DestroyOnHit = true;
	SphereCollisionComp->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	//SphereCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlapBegin);

	bReplicates = true;
	//ShouldMulticastOnHit = true;

	LifeTime = 20.0f;
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	bReplicateMovement = MovementOnServer;
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Count down the life time until the projectile should be destroyed
	(LifeTime > 0.0f) ? (LifeTime -= DeltaTime) : Destroy();

}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// Make sure the projectile is on the server to continue
	if (Role < ROLE_Authority)
	{
		ClientShouldHide = true;
		return;
	}

	// If both the actor hit and the instigator is ShooterCharacters.
	if (OtherActor->ActorHasTag("ShooterCharacter") && (InstigatorCharacter && InstigatorCharacter->ActorHasTag("ShooterCharacter")) && !IsExplosionProjectile)
	{

		// First make sure they are of a different faction
		AShooterCharacterBase* CharacterHit = Cast<AShooterCharacterBase>(OtherActor);
		if (CharacterHit && CharacterHit->Faction != Faction)
		{
			bool WasCrit = false;
			// Calculate how much damage this projectile will do
			float Damage = UShooterStaticLibrary::CalculateDamageCharacter(	CharacterHit, 
																			InstigatorCharacter, 
																			DamageLow, DamageHigh, CritChance, CritDamage, DamageType, WasCrit);

			// Apply the damage to the character
			UShooterStaticLibrary::ApplyDamageCharacter(CharacterHit, InstigatorCharacter, Damage, DamageType, WasCrit);
		}
	}
	
	// Depending on if the hit should be multicasted or not, call different functions.
	//(ShouldMulticastOnHit == true) ? MulticastProjHit(OtherActor, GetActorLocation()) : BPProjectileHit(OtherActor, GetActorLocation());

	// If the projectile is one that explodes, tell the projectile that it has hit something. Else just tell all the projectiles that the server has been hit
	if (IsExplosionProjectile)
		HitExplosion(OtherActor, GetActorLocation());
	else
		MulticastProjHit(OtherActor, GetActorLocation());

	// Only destroy if it's not a explosion projectile
	if (DestroyOnHit && !IsExplosionProjectile)
		Destroy();
}

void ABaseProjectile::HitExplosion(AActor* HitActor, FVector Location)
{
	//PrintText("HitExplosion Base", -1, 2.0f, FColor::Green);
}

void ABaseProjectile::ProjectileExplode()
{
	if (Role < ROLE_Authority || ExplosionClass == nullptr)
		return;

	// Spawn the explosion, set needed variables, create the effect, damage characters if there are any, then destroy the explosion.
	AExplosion* SpawnedExplosion = SpawnBP<AExplosion>(GetWorld(), ExplosionClass, GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f));
	SpawnedExplosion->Faction = Faction;
	SpawnedExplosion->InstigatorCharacter = InstigatorCharacter;
	SpawnedExplosion->SetDamageStats(DamageType, DamageLow, DamageHigh, CritChance, CritDamage);
	SpawnedExplosion->ExplodeEffect(DamageTypeColor, ExplosionRadius);
	SpawnedExplosion->ExplodeArea(ExplosionRadius);
	//SpawnedExplosion->Destroy();

	// Destroy the projectile
	Destroy();
}

void ABaseProjectile::SetInitialValues(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, FName ProfileName)
{
	// Since this is a multicast function we are calling, make sure we are on the server.
	if (Role == ROLE_Authority)
	{
		// If the movement is not replicated, set the velocity on all the projectiles
		if (!MovementOnServer)
		{
			MulticastSetInitialValues(InDamageType, InDamageTypeColor, ProjectileVelocity, true, ProfileName);
		}
		else if (MovementOnServer)
		{
			// Set the velocity on the server projectile only if the movement should be replicated.
			ProjectileMovement->SetVelocityInLocalSpace(ProjectileVelocity);
			MulticastSetInitialValues(InDamageType, InDamageTypeColor, ProjectileVelocity, false, ProfileName);
		}
	}
}

void ABaseProjectile::MulticastSetInitialValues_Implementation(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, bool SetVelocity, FName ProfileName)
{
	SphereCollisionComp->SetCollisionProfileName(ProfileName);

	// Set the values used to set the various effect colors
	DamageType = InDamageType;
	DamageTypeColor = InDamageTypeColor;

	// Call the BP function that sets some of the colors
	BPSetColors();

	// Set the velocity of the spawned projectiles
	if(SetVelocity)
		ProjectileMovement->SetVelocityInLocalSpace(ProjectileVelocity);
}

bool ABaseProjectile::MulticastSetInitialValues_Validate(EDamageTypes InDamageType, FLinearColor InDamageTypeColor, FVector ProjectileVelocity, bool SetVelocity, FName ProfileName)
{
	return true;
}

void ABaseProjectile::MulticastProjHit_Implementation(AActor* OtherActor, FVector Location)
{
	BPProjectileHit(OtherActor, Location);
}

bool ABaseProjectile::MulticastProjHit_Validate(AActor* OtherActor, FVector Location)
{
	return true;
}