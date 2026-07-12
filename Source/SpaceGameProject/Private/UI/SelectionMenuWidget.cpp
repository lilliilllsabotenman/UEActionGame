// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SelectionMenuWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "TextFileParser.h"
#include "LineDirectiveParser.h"
#include "SpaceGameProject.h"
#include "Kismet/GameplayStatics.h"

TSharedRef<SWidget> USelectionMenuWidget::RebuildWidget()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Root;

	LineContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());

	if (UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(LineContainer))
	{
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		CanvasSlot->SetOffsets(FMargin(0.f));
	}

	ParseMenuFile();
	BuildLineWidgets();
	RefreshHighlight();

	return Super::RebuildWidget();
}

void USelectionMenuWidget::ParseMenuFile()
{
	ParsedLines.Reset();
	MaxItemIndex = INDEX_NONE;

	for (const FString& RawLine : TextFileParser::ParseLines(MenuFilePath.FilePath))
	{
		FSelectionMenuLine Line;

		FLineDirective Directive;
		FString Content;
		if (!LineDirectiveParser::TryParse(RawLine, Directive, Content))
		{
			Line.Type = ESelectionMenuLineType::PlainText;
			Line.Text = RawLine;
			ParsedLines.Add(Line);
			continue;
		}

		if (Directive.Marker == TEXT("ITEM") && Directive.ParamType == ELineDirectiveParamType::Int)
		{
			Line.Type = ESelectionMenuLineType::Item;
			Line.Text = Content;
			Line.ItemIndex = Directive.IntParam;
			MaxItemIndex = FMath::Max(MaxItemIndex, Line.ItemIndex);
		}
		else
		{
			UE_LOG(LogSpaceGameProject, Error, TEXT("SelectionMenuWidget: unknown directive '%s' in line '%s'"), *Directive.Marker, *RawLine);
			Line.Type = ESelectionMenuLineType::PlainText;
			Line.Text = Content;
		}

		ParsedLines.Add(Line);
	}
}

void USelectionMenuWidget::BuildLineWidgets()
{
	for (FSelectionMenuLine& Line : ParsedLines)
	{
		UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = FontSize;
		TextBlock->SetFont(FontInfo);
		TextBlock->SetText(FText::FromString(Line.Text));

		LineContainer->AddChildToVerticalBox(TextBlock);

		Line.TextBlock = TextBlock;
	}
}

void USelectionMenuWidget::RefreshHighlight()
{
	for (const FSelectionMenuLine& Line : ParsedLines)
	{
		if (Line.Type != ESelectionMenuLineType::Item || !Line.TextBlock) continue;

		Line.TextBlock->SetColorAndOpacity(Line.ItemIndex == CurrentIndex ? HighlightColor : NormalColor);
	}
}

void USelectionMenuWidget::NavigateUp()
{
	const int32 NewIndex = CurrentIndex - 1;
	if (NewIndex < 0) return;

	CurrentIndex = NewIndex;
	RefreshHighlight();
}

void USelectionMenuWidget::NavigateDown()
{
	const int32 NewIndex = CurrentIndex + 1;
	if (NewIndex > MaxItemIndex) return;

	CurrentIndex = NewIndex;
	RefreshHighlight();
}

void USelectionMenuWidget::Confirm()
{
	OnSelectionConfirmed.Broadcast(CurrentIndex);

	const FSelectionLevelEntry* Entry = LevelsByIndex.FindByPredicate([this](const FSelectionLevelEntry& InEntry)
	{
		return InEntry.Index == CurrentIndex;
	});

	if (!Entry)
	{
		UE_LOG(LogSpaceGameProject, Error, TEXT("SelectionMenuWidget: no level mapped for Index %d"), CurrentIndex);
		return;
	}

	UGameplayStatics::OpenLevel(this, FName(*Entry->LevelName));
}
