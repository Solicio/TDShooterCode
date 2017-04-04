// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "PlayerHUDWidget.h"

#include "CharacterScreen/CharacterScreenWidget.h"
#include "Stash/StashWidget.h"

UPlayerHUDWidget::UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FontSizeNormal = 26;
	FontSizeCrit = 36;
	CombatTextSpeed = 100.0f;
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	// Reduce the duration until it reaches 0, then delete it.
	for (int i = FloatingCombatTexts.Num() - 1; i >= 0; i--)
	{
		if (FloatingCombatTexts[i].CurrentDuration <= 0.0f)
		{
			FloatingCombatTexts.RemoveAt(i);
		}
		else
		{
			FloatingCombatTexts[i].CurrentDuration -= DeltaTime;

			if(!FloatingCombatTexts[i].IsCrit)
				FloatingCombatTexts[i].DrawLocation.Z += DeltaTime * CombatTextSpeed;
		}
		
	}
}

void UPlayerHUDWidget::NewCombatText(FString CombatText, FVector Location, EDamageTypes DamageType, bool IsCrit)
{
	// Create a new struct and fill it with the required information
	FCombatTextElement NewElement;
	NewElement.CombatText = CombatText;
	NewElement.CombatText.RemoveFromEnd(".0");

	NewElement.DrawLocation = Location;
	float RandX = FMath::RandRange(-50.0f, 50.0f);
	float Randy = FMath::RandRange(-50.0f, 50.0f);

	NewElement.DrawLocation.X += RandX;
	NewElement.DrawLocation.Y += Randy;
	NewElement.DrawLocation.Z += 190.0f;
	
	NewElement.CurrentDuration = 1.0f;
	NewElement.FontSize = FontSizeNormal;

	if (ColorData)
	{
		switch (DamageType)
		{
			case EDamageTypes::DT_Fire:
				NewElement.TextColor = ColorData->Colors.Fire;
				break;

			case EDamageTypes::DT_Ice:
				NewElement.TextColor = ColorData->Colors.Ice;
				break;

			case EDamageTypes::DT_Poison:
				NewElement.TextColor = ColorData->Colors.Poison;
				break;

			case EDamageTypes::DT_Lightning:
				NewElement.TextColor = ColorData->Colors.Lightning;
				break;

			case EDamageTypes::DT_Arcane:
				NewElement.TextColor = ColorData->Colors.Arcane;
				break;

			default:
				NewElement.TextColor = FColor::White;
				break;
		};
	}
	else
	{
		NewElement.TextColor = FColor::White;
	}

	// Crit dependant
	if (IsCrit)
	{
		NewElement.FontSize = FontSizeCrit;
		NewElement.CombatText += "!";
	}
	NewElement.IsCrit = IsCrit;

	// Measure the string to see how large it is.
	if (CombatTextFont.HasValidFont())
	{
		CombatTextFont.Size = NewElement.FontSize;
		TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		NewElement.TextSize = FontMeasure->Measure(NewElement.CombatText, CombatTextFont);
	}

	// If the text is a crit, add to the end of the array to draw it last.
	if (NewElement.IsCrit)
	{
		FloatingCombatTexts.Add(NewElement);
	}
	else
	{
		for (int i = FloatingCombatTexts.Num() - 1; i >= 0; i--)
		{
			// The first element that is not a crit, insert the element here
			if (!FloatingCombatTexts[i].IsCrit)
			{
				FloatingCombatTexts.Insert(NewElement, i);
				return;
			}
		}

		// Array is empty or all elements are crits.
		FloatingCombatTexts.Insert(NewElement, 0);
	}
	
}

void UPlayerHUDWidget::UpdateCharacterScreen()
{
	if (CharacterScreenWidget == nullptr)
		return;

	CharacterScreenWidget->UpdateScreen();
	RemoveWeaponTooltips();
}

void UPlayerHUDWidget::UpdateStashWidget()
{
	// Only update if it's showing
	if (StashWidget == nullptr || !StashWidgetVisible)
		return;

	StashWidget->UpdateStash();
	RemoveWeaponTooltips();
}

void UPlayerHUDWidget::ToggleStashWidget()
{
	if (StashWidget == nullptr)
		return;

	// Update if it is going to show after this function
	if (!StashWidgetVisible)
	{
		StashWidget->UpdateStash();
		RemoveWeaponTooltips();
	}

	// Toggle the widget
	StashWidgetVisible = !StashWidgetVisible;
}

void UPlayerHUDWidget::SetControllerAndCharacter()
{
	OwningController = Cast<AShooterPC>(GetOwningPlayer());
	OwningCharacter = Cast<AShooterCharacter>(GetOwningPlayerPawn());
}

AShooterPC* UPlayerHUDWidget::GetShooterPC()
{
	return OwningController;
}

AShooterCharacter* UPlayerHUDWidget::GetShooterCharacter()
{
	return OwningCharacter;
}
