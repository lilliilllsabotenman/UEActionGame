// Fill out your copyright notice in the Description page of Project Settings.

#include "WarningWidgetComponent.h"
#include "UI/BlinkTextWidget.h"
#include "Blueprint/UserWidget.h"

UWarningWidgetComponent::UWarningWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWarningWidgetComponent::StartWarning(const FString& Content)
{
	if (!WarningWidgetClass) return;

	if (!WidgetInstance)
	{
		WidgetInstance = CreateWidget<UBlinkTextWidget>(GetWorld(), WarningWidgetClass);
		if (!WidgetInstance) return;

		WidgetInstance->AddToViewport();
	}

	WidgetInstance->SetBlinkText(FText::FromString(Content));

	if (!bIsWarningActive)
	{
		WidgetInstance->StartBlink(BlinkInterval);
		bIsWarningActive = true;
	}
}

void UWarningWidgetComponent::StopWarning()
{
	if (!bIsWarningActive) return;

	if (WidgetInstance)
	{
		WidgetInstance->StopBlink(false);
	}

	bIsWarningActive = false;
}
