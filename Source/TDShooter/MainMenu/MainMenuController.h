// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/MainMenu/MainMenuWidget.h"

#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()
	
	public:

		/** Main menu widget class to be spawned */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
		TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

		/** Reference to the main menu */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
		UMainMenuWidget* MainMenu;


	protected:

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;	
};
