// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "PT_GrenadeLauncher.h"


// Called when the game starts or when spawned
void APT_GrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentDelay = ExplosionDelay;
}

// Called every frame
void APT_GrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Simply count down and then call the BP event.
	if (Role == ROLE_Authority)
	{
		if (CurrentDelay > 0.0f)
		{
			CurrentDelay -= DeltaTime;
		}
		else
		{
			ProjectileExplode();
			CurrentDelay = 3.0f;
		}
	}
}

void APT_GrenadeLauncher::HitExplosion(AActor* HitActor, FVector Location)
{
	if (Role == ROLE_Authority)
	{
		// Explode if hit someone of different faction
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if(HitCharacter && (UShooterStaticLibrary::GetCharacterFaction(HitCharacter) != EFactions::F_None) && (UShooterStaticLibrary::GetCharacterFaction(HitCharacter) != Faction))
			ProjectileExplode();
	}
}