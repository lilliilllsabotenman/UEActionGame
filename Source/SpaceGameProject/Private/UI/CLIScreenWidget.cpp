// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/CLIScreenWidget.h"

void UCLIScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LogWidget->OnLogPlaybackFinished.AddDynamic(this, &UCLIScreenWidget::HandleLogPlaybackFinished);
	MenuWidget->OnSelectionConfirmed.AddDynamic(this, &UCLIScreenWidget::HandleSelectionConfirmed);

	CurrentPhase = ECLIScreenPhase::Intro;
	LogWidget->SetVisibility(ESlateVisibility::Visible);
	MenuWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UCLIScreenWidget::HandleLogPlaybackFinished()
{
	CurrentPhase = ECLIScreenPhase::Selecting;
	LogWidget->SetVisibility(ESlateVisibility::Collapsed);
	MenuWidget->SetVisibility(ESlateVisibility::Visible);
}

void UCLIScreenWidget::HandleSelectionConfirmed(ECLIMenuOptionKey SelectedKey)
{
	CurrentPhase = ECLIScreenPhase::Confirmed;
	OnCLIScreenConfirmed.Broadcast(SelectedKey);
}