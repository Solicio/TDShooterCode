// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Lobby/LobbyScreenWidget.h"

#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:
	
		/** Main menu widget class to be spawned */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
		TSubclassOf<ULobbyScreenWidget> LobbyScreenWidgetClass;

		/** Reference to the main menu */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
		ULobbyScreenWidget* LobbyScreen;
	

	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;
};
