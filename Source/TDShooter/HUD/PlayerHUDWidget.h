// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Characters/Player/ShooterPC.h"
#include "../Characters/Player/ShooterCharacter.h"

#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UCharacterScreenWidget;
class UStashWidget;

/** Used to store the floating combat text that should be shown in the world */
USTRUCT(BlueprintType)
struct FCombatTextElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FString CombatText;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	//float MaxDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	float CurrentDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FVector DrawLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	int32 FontSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FLinearColor TextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	FVector2D TextSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct")
	bool IsCrit;
};

/** This is the main widget that the player has.
 *	All other widgets that the player will have on the screen will be placed in this class.
 *	The other widgets that this widget will contain should inherit from ShooterWidgetBase
 */
UCLASS()
class TDSHOOTER_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
	
		UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer);

		/** Collection of all the different colors used for the different damage types */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerHUD")
		UDamageColorData* ColorData;

		/** If the player is hovering a peice of widget that should change the cursor back to the default arrow. */
		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerHUD")
		//bool IsHoveringWidgets;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerHUD")
		bool LeftButtonDown;


		//--- Floating Combat Text Functions ---//

		/** Creates a new struct element to add to the array of floating combat texts */
		UFUNCTION(BlueprintCallable, Category = "Floating Combat Text")
		void NewCombatText(FString CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit);

		/** Array that stores all the current combat texts */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floating Combat Text")
		TArray<FCombatTextElement> FloatingCombatTexts;

		/** Font size for the combat text */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Combat Text")
		int32 FontSizeNormal;

		/** Font size of the combat text for crits */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Combat Text")
		int32 FontSizeCrit;

		/** How fast the text will be moving upwards */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Combat Text")
		float CombatTextSpeed;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Combat Text")
		FSlateFontInfo CombatTextFont;


		//--- Show weapon tooltip ---//

		/** Tells the HUD to show a weapon tooltip in the bottom right corner, used when hovering over loot containers */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon Tooltip")
		void ShowWeaponTooltip(FStoredWeapon StoredWeapon);

		/** Tells the HUD to hide the weapon tooltip that is used for loot containers */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon Tooltip")
		void HideWeaponTooltip();

		/** Removes all weaon tooltips from the HUD */
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon Tooltip")
		void RemoveWeaponTooltips();

		/** Reference to the active tooltip widget for weapons, should be used for all tooltips. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tooltip")
		UUserWidget* WeaponTooltipRef;


		//--- Character Screen ---//

		/** If the character screen should be visible or not */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		bool CharacterScreenVisible;

		/** Reference to the character screen, set in BP */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Screen")
		UCharacterScreenWidget* CharacterScreenWidget;

		/** Updates the character screen widget */
		UFUNCTION(BlueprintCallable, Category = "Character Screen")
		void UpdateCharacterScreen();


		//--- Character Screen ---//

		/** If the stash widget should be visible or not */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stash")
		bool StashWidgetVisible;

		/** Reference to the stash widget, set in BP */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stash")
		UStashWidget* StashWidget;

		/** Updates the stash widget */
		UFUNCTION(BlueprintCallable, Category = "Stash")
		void UpdateStashWidget();

		UFUNCTION(BlueprintCallable, Category = "Stash")
		void ToggleStashWidget();


		//--- General Functions ---//

		/** Casts and stores the controller and character to the base classes */
		void SetControllerAndCharacter();

		/** Get the player controller casted to the ShooterPC class */
		UFUNCTION(BlueprintPure, Category = "PlayerHUD")
		AShooterPC* GetShooterPC();

		/** Get the player character casted to the ShooterCharacter class */
		UFUNCTION(BlueprintPure, Category = "PlayerHUD")
		AShooterCharacter* GetShooterCharacter();


	protected:

		UPROPERTY()
		AShooterPC* OwningController;

		UPROPERTY()
		AShooterCharacter* OwningCharacter;

		/** Called every frame */
		virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
