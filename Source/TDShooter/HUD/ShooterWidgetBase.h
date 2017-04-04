// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerHudWidget.h"

#include "Blueprint/UserWidget.h"
#include "ShooterWidgetBase.generated.h"

/** 
 *	This is the base class of all the widgets that will be inside the PlayerHUDWidget.
 *	Mainly used for larger widgets such as CharacterScreen, the stat overlay.
 *	Could be used for smaller ones too.
 *	The playerhud sets all the references by it's own.
 */
UCLASS()
class TDSHOOTER_API UShooterWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
	public:

		UShooterWidgetBase(const FObjectInitializer& ObjectInitializer);

		/** Sets the reference to the most outward parent, the PlayerHUDWidget */
		UFUNCTION(BlueprintCallable, Category = "Shooter Widget Base")
		void SetPlayerHUDWidget(UPlayerHUDWidget* Widget);

		/** Gets the reference to the most outward parent, the PlayerHUDWidget */
		UFUNCTION(BlueprintPure, Category = "Shooter Widget Base")
		UPlayerHUDWidget* GetPlayerHUDWidget();


	protected:

		UPROPERTY()
		UPlayerHUDWidget* PlayerHUDWidget;
};
