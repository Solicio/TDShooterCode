// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "ShooterWidgetBase.h"


UShooterWidgetBase::UShooterWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UShooterWidgetBase::SetPlayerHUDWidget(UPlayerHUDWidget* Widget)
{
	PlayerHUDWidget = Widget;
}

UPlayerHUDWidget* UShooterWidgetBase::GetPlayerHUDWidget()
{
	return PlayerHUDWidget;
}