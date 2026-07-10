// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionMenuWidget.h"
#include "Components/TextBlock.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

void USelectionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MenuMappingContext, 0);
		}

		if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			EnhancedInput->BindAction(MoveUpAction, ETriggerEvent::Started, this, &USelectionMenuWidget::HandleMoveUp);
			EnhancedInput->BindAction(MoveDownAction, ETriggerEvent::Started, this, &USelectionMenuWidget::HandleMoveDown);
			EnhancedInput->BindAction(ConfirmAction, ETriggerEvent::Started, this, &USelectionMenuWidget::HandleConfirm);
		}
	}

	RefreshHighlight();
}

void USelectionMenuWidget::NativeDestruct()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(MenuMappingContext);
		}
	}

	Super::NativeDestruct();
}

void USelectionMenuWidget::HandleMoveUp(const FInputActionValue& Value)
{
	SelectedIndex = FMath::Clamp(SelectedIndex - 1, 0, OptionLabels.Num() - 1);
	RefreshHighlight();
}

void USelectionMenuWidget::HandleMoveDown(const FInputActionValue& Value)
{
	SelectedIndex = FMath::Clamp(SelectedIndex + 1, 0, OptionLabels.Num() - 1);
	RefreshHighlight();
}

void USelectionMenuWidget::HandleConfirm(const FInputActionValue& Value)
{
	if (!OptionKeys.IsValidIndex(SelectedIndex)) return;

	OnSelectionConfirmed.Broadcast(OptionKeys[SelectedIndex]);
}

void USelectionMenuWidget::RefreshHighlight()
{
	for (int32 Index = 0; Index < OptionLabels.Num(); ++Index)
	{
		if (UTextBlock* Label = OptionLabels[Index])
		{
			Label->SetColorAndOpacity(Index == SelectedIndex ? HighlightColor : NormalColor);
		}
	}
}
