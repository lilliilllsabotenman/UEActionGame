// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TextLogWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "TextFileParser.h"
#include "LineDirectiveParser.h"
#include "SpaceGameProject.h"

TSharedRef<SWidget> UTextLogWidget::RebuildWidget()
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Root;

	LineContainer = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());

	if (UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(LineContainer))
	{
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		CanvasSlot->SetOffsets(FMargin(0.f));
	}

	return Super::RebuildWidget();
}

void UTextLogWidget::DoAnimation(const FOnAnimationFinished& OnFinished)
{
	PendingFinishedCallback = OnFinished;

	const TArray<FString> RawLines = TextFileParser::ParseLines(LogFilePath.FilePath);
	DisplayLines(InterpretTextData(RawLines, DefaultTextData), LineInterval);
}

TArray<FTextData> UTextLogWidget::InterpretTextData(const TArray<FString>& RawLines, const FTextData& DefaultTextData)
{
	TArray<FTextData> Result;
	Result.Reserve(RawLines.Num());

	for (const FString& RawLine : RawLines)
	{
		FTextData Data = DefaultTextData;

		FLineDirective Directive;
		FString Content;
		if (!LineDirectiveParser::TryParse(RawLine, Directive, Content))
		{
			Data.Text = RawLine;
			Result.Add(Data);
			continue;
		}

		if (Directive.Marker == TEXT("COLOR") && Directive.ParamType == ELineDirectiveParamType::String)
		{
			TArray<FString> Components;
			Directive.StringParam.ParseIntoArray(Components, TEXT(","));

			if (Components.Num() == 3)
			{
				const uint8 R = static_cast<uint8>(FCString::Atoi(*Components[0]));
				const uint8 G = static_cast<uint8>(FCString::Atoi(*Components[1]));
				const uint8 B = static_cast<uint8>(FCString::Atoi(*Components[2]));
				Data.Color = FLinearColor(FColor(R, G, B));
			}
			else
			{
				UE_LOG(LogSpaceGameProject, Error, TEXT("TextLogWidget: COLOR directive expects 'R,G,B', got '%s' in line '%s'"), *Directive.StringParam, *RawLine);
			}
		}
		else if (Directive.Marker == TEXT("SIZE") && Directive.ParamType == ELineDirectiveParamType::Int)
		{
			Data.FontSize = Directive.IntParam;
		}
		else
		{
			UE_LOG(LogSpaceGameProject, Error, TEXT("TextLogWidget: unknown directive '%s' in line '%s'"), *Directive.Marker, *RawLine);
		}

		Data.Text = Content;
		Result.Add(Data);
	}

	return Result;
}

void UTextLogWidget::DisplayLines(const TArray<FTextData>& Lines, float Interval)
{
	if (!LineContainer) return;

	PendingLines.Append(Lines);

	if (Interval <= 0.f)
	{
		while (NextPendingIndex < PendingLines.Num())
		{
			RevealNextPendingLine();
		}
		return;
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(LineRevealTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(LineRevealTimerHandle, this, &UTextLogWidget::RevealNextPendingLine, Interval, true);
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

	const FTextData& Data = PendingLines[NextPendingIndex];

	UTextBlock* Line = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

	FSlateFontInfo FontInfo = Line->GetFont();
	FontInfo.Size = Data.FontSize;
	Line->SetFont(FontInfo);
	Line->SetText(FText::FromString(Data.Text));
	Line->SetColorAndOpacity(Data.Color);

	LineContainer->AddChild(Line);
	LineContainer->ScrollToEnd();

	++NextPendingIndex;

	if (NextPendingIndex >= PendingLines.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(LineRevealTimerHandle);
		PendingFinishedCallback.ExecuteIfBound();
	}
}