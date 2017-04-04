// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "PT_RocketLauncher.h"


// Called when the game starts or when spawned
void APT_RocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APT_RocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APT_RocketLauncher::HitExplosion(AActor* HitActor, FVector Location)
{
	if (Role == ROLE_Authority)
	{
		ProjectileExplode();
	}

	//PrintText("HitExplosion Rocket", -1, 2.0f, FColor::Green);
}