// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/CLIMenuWidget.h"
#include "Components/TextBlock.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

void UCLIMenuWidget::NativeConstruct()
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
			EnhancedInput->BindAction(MoveUpAction, ETriggerEvent::Started, this, &UCLIMenuWidget::HandleMoveUp);
			EnhancedInput->BindAction(MoveDownAction, ETriggerEvent::Started, this, &UCLIMenuWidget::HandleMoveDown);
			EnhancedInput->BindAction(ConfirmAction, ETriggerEvent::Started, this, &UCLIMenuWidget::HandleConfirm);
		}
	}

	RefreshHighlight();
}

void UCLIMenuWidget::NativeDestruct()
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

void UCLIMenuWidget::HandleMoveUp(const FInputActionValue& Value)
{
	SelectedIndex = FMath::Clamp(SelectedIndex - 1, 0, OptionLabels.Num() - 1);
	RefreshHighlight();
}

void UCLIMenuWidget::HandleMoveDown(const FInputActionValue& Value)
{
	SelectedIndex = FMath::Clamp(SelectedIndex + 1, 0, OptionLabels.Num() - 1);
	RefreshHighlight();
}

void UCLIMenuWidget::HandleConfirm(const FInputActionValue& Value)
{
	OnSelectionConfirmed.Broadcast(OptionKeys[SelectedIndex]);
}

void UCLIMenuWidget::RefreshHighlight()
{
	for (int32 Index = 0; Index < OptionLabels.Num(); ++Index)
	{
		OptionLabels[Index]->SetColorAndOpacity(Index == SelectedIndex ? HighlightColor : NormalColor);
	}
}