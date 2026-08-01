// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BlinkTextWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

TSharedRef<SWidget> UBlinkTextWidget::RebuildWidget()
{
	RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootPanel;

	CenterText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

	if (UCanvasPanelSlot* CanvasSlot = RootPanel->AddChildToCanvas(CenterText))
	{
		CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
	}

	return Super::RebuildWidget();
}

void UBlinkTextWidget::SetBlinkText(const FText& NewText)
{
	if (CenterText)
	{
		CenterText->SetText(NewText);
	}
}

void UBlinkTextWidget::StartBlink(float Interval)
{
	if (!RootPanel) return;

	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UBlinkTextWidget::ToggleBlinkVisibility, Interval, true);
}

void UBlinkTextWidget::StopBlink(bool bLeaveVisible)
{
	GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);

	if (RootPanel)
	{
		RootPanel->SetVisibility(bLeaveVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UBlinkTextWidget::ToggleBlinkVisibility()
{
	if (!RootPanel) return;

	const bool bCurrentlyVisible = RootPanel->GetVisibility() != ESlateVisibility::Collapsed;
	RootPanel->SetVisibility(bCurrentlyVisible ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
}
