// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "MainMenuController.h"

void AMainMenuController::BeginPlay()
{

	// Spawn the HUD Widget, make sure to only spawn it on the local controller
	if (IsLocalPlayerController() && MainMenuWidgetClass != nullptr)
	{
		MainMenu = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);

		// Add it to viewport
		if (MainMenu)
		{
			MainMenu->AddToViewport();
		}
	}

	// Call after widget creation to make sure that the BP begin play has a valid menu
	Super::BeginPlay();
}

void AMainMenuController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}