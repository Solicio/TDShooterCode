// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "LobbyPlayerController.h"


void ALobbyPlayerController::BeginPlay()
{
	// Spawn the HUD Widget, make sure to only spawn it on the local controller
	if (IsLocalPlayerController() && LobbyScreenWidgetClass != nullptr)
	{
		LobbyScreen = CreateWidget<ULobbyScreenWidget>(this, LobbyScreenWidgetClass);

		// Add it to viewport
		if (LobbyScreen)
		{
			LobbyScreen->AddToViewport();
		}
	}

	// Call after widget creation to make sure that the BP begin play has a valid menu
	Super::BeginPlay();
}

void ALobbyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}