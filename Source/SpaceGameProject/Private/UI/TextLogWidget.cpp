// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TextLogWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "TextFileParser.h"

TSharedRef<SWidget> UTextLogWidget::RebuildWidget()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Root;

	TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.Size = FontSize;
	TextBlock->SetFont(FontInfo);

	if (UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(TextBlock))
	{
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		CanvasSlot->SetOffsets(FMargin(0.f));
	}

	return Super::RebuildWidget();
}

void UTextLogWidget::DoAnimation(const FOnAnimationFinished& OnFinished)
{
	PendingFinishedCallback = OnFinished;
	DisplayLines(TextFileParser::ParseLines(LogFilePath.FilePath));
}

void UTextLogWidget::DisplayLines(const TArray<FString>& Lines)
{
	if (!TextBlock) return;

	PendingLines.Append(Lines);

	if (LineInterval <= 0.f)
	{
		while (NextPendingIndex < PendingLines.Num())
		{
			RevealNextPendingLine();
		}
		return;
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(LineRevealTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(LineRevealTimerHandle, this, &UTextLogWidget::RevealNextPendingLine, LineInterval, true);
	}
}

void UTextLogWidget::RevealNextPendingLine()
{
	if (NextPendingIndex >= PendingLines.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(LineRevealTimerHandle);
		PendingFinishedCallback.ExecuteIfBound();
		return;
	}

	FString Combined = TextBlock->GetText().ToString();
	if (!Combined.IsEmpty())
	{
		Combined += TEXT("\n");
	}
	Combined += PendingLines[NextPendingIndex];
	TextBlock->SetText(FText::FromString(Combined));

	++NextPendingIndex;

	if (NextPendingIndex >= PendingLines.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(LineRevealTimerHandle);
		PendingFinishedCallback.ExecuteIfBound();
	}
}
