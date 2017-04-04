// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UENUM(BlueprintType)
enum class EMenuStates : uint8
{
	MainButtons,
	StartGame,
	HostGame,
	JoinServer,
	Options
};

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu")
		EMenuStates CurrentState;
	
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Main Menu")
		void SetMenuState(EMenuStates State);
};
