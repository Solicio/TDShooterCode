// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "Explosion.h"

#include "Characters/ShooterCharacterBase.h"

// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	/*ExplosionRadius = 0.0f;*/
	CheckLineOfSight = true;

	SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComp"));
	SphereCollisionComp->SetupAttachment(RootComponent);
	//SphereCollisionComp->SetSphereRadius(ExplosionRadius);

	ExplosionLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("ExplosionLightComp"));
	ExplosionLightComp->SetupAttachment(RootComponent);
	ExplosionLightComp->SetIntensity(5000.0f);
	ExplosionLightComp->SetAttenuationRadius(1000.0f);

	bReplicates = true;

	DestroyDelayMax = 0.3f;
	DestroyDelay = 10.0f;
}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();
	
	//ExplodeArea();
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		if (DestroyDelay > 0.0f)
			DestroyDelay -= DeltaTime;
		else if (DestroyDelay <= 0.0f)
			Destroy();
	}
}

void AExplosion::SetDamageStats(EDamageTypes Type, float DmgLow, float DmgHigh, float CritC, float CritD)
{
	// The projectile sets all the needed variables
	DamageType = Type;
	DamageLow = DmgLow;
	DamageHigh = DmgHigh;
	CritChance = CritC;
	CritDamage = CritD;
}

void AExplosion::ExplodeArea(float ExplosionRad)
{
	// Make sure we are on server to continue
	if (Role < ROLE_Authority)
		return;

	// First set the radius to the right size
	SphereCollisionComp->SetSphereRadius(ExplosionRad);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRad, 10, FColor::Blue, false, DestroyDelayMax);

	// Check for stuff to damage
	//PrintText("DmgType" + UShooterStaticLibrary::GetDamageTypeEnumAsString(DamageType) + ", DmgLow: " + FString::SanitizeFloat(DamageLow) + ", DmgHigh: " + FString::SanitizeFloat(DamageHigh) + ", CritC: " + FString::SanitizeFloat(CritChance) + ", CritD: " + FString::SanitizeFloat(CritDamage), -1, 5.0f, FColor::Red);

	TArray<AActor*> OverlappingActors;
	SphereCollisionComp->GetOverlappingActors(OverlappingActors, AShooterCharacterBase::StaticClass());

	// Look through all the overlapping actors to see if there is any characters that can be hit
	for (auto Itr(OverlappingActors.CreateIterator()); Itr; Itr++)
	{
		// First make sure they are of a different faction
		if ((*Itr) && (*Itr)->ActorHasTag("ShooterCharacter"))
		{
			AShooterCharacterBase* CharacterHit = Cast<AShooterCharacterBase>((*Itr));
			if (CharacterHit && CharacterHit->Faction != Faction)
			{
				bool WasCrit = false;
				// Calculate how much damage this explosion will do
				float Damage = UShooterStaticLibrary::CalculateDamageCharacter(	CharacterHit,
																				Cast<AShooterCharacterBase>(InstigatorCharacter),
																				DamageLow, DamageHigh, CritChance, CritDamage, DamageType, WasCrit);

				// Apply the damage to the character
				UShooterStaticLibrary::ApplyDamageCharacter(CharacterHit, Cast<AShooterCharacterBase>(InstigatorCharacter), Damage, DamageType, WasCrit);
			}
		}
	}
}

void AExplosion::ExplodeEffect(FLinearColor EffectColor, float ExplosionRad)
{
	if (Role == ROLE_Authority)
	{
		MulticastExplodeAreaEffect(EffectColor);
		DestroyDelay = DestroyDelayMax;
	}
}

void AExplosion::MulticastExplodeAreaEffect_Implementation(FLinearColor EffectColor)
{
	if (ExplosionParticleSystem != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, GetActorLocation(), FRotator::ZeroRotator, true)->SetColorParameter("Param_ParticleColor", EffectColor);
	}

	if (ExplosionSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	ExplosionLightComp->SetLightColor(EffectColor);
}

bool AExplosion::MulticastExplodeAreaEffect_Validate(FLinearColor EffectColor)
{
	return true;
}