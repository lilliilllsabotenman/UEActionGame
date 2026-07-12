// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"
#include "AnimationWidget.h"
#include "TextLogWidget.generated.h"

class UVerticalBox;
class UTextBlock;

// Fully-resolved display data for a single log line, as produced by UTextLogWidget::InterpretTextData.
USTRUCT(BlueprintType)
struct FTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextLog")
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextLog")
	FLinearColor Color = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextLog", meta = (ClampMin = "1"))
	int32 FontSize = 24;
};

/**
 *  Self-contained screen-filling text log.
 *  Builds its own full-screen VerticalBox and loads LogFilePath into it when DoAnimation is
 *  called, adding one TextBlock per revealed line.
 */
UCLASS(abstract)
class SPACEGAMEPROJECT_API UTextLogWidget : public UUserWidget, public IAnimationWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void DoAnimation(const FOnAnimationFinished& OnFinished) override;
	// Appends each line to the log, in order. Interval is the delay in seconds between each
	// revealed line; 0 reveals every queued line immediately.
	UFUNCTION(BlueprintCallable, Category = "TextLog")
	void DisplayLines(const TArray<FTextData>& Lines, float Interval);

	// Interprets raw file lines into ready-to-render FTextData. A line with no directive gets
	// DefaultTextData's Color/FontSize as-is. A "[COLOR:R,G,B]" or "[SIZE:N]" directive overrides
	// just that one field for its line; any other/malformed directive is logged and the line
	// falls back to DefaultTextData's styling.
	static TArray<FTextData> InterpretTextData(const TArray<FString>& RawLines, const FTextData& DefaultTextData);

protected:

	/** Text file (relative to the project directory) loaded into the log on construct */
	UPROPERTY(EditAnywhere, Category = "TextLog", meta = (RelativeToGameDir, FilePathFilter = "txt"))
	FFilePath LogFilePath;

	// Interval DoAnimation passes to DisplayLines. See DisplayLines for what it means.
	UPROPERTY(EditAnywhere, Category = "TextLog", meta = (ClampMin = "0.0"))
	float LineInterval = 0.3f;

	// Color/FontSize applied to every line until the interpretation step can override them per
	// line. The Text field here is unused (each line's Text is filled in separately).
	UPROPERTY(EditAnywhere, Category = "TextLog")
	FTextData DefaultTextData;

	// Builds the CanvasPanel/VerticalBox tree before Super converts WidgetTree->RootWidget to Slate.
	// Doing this in NativeConstruct instead is too late: RebuildWidget runs first and would ship
	// the old (empty) root to the screen, leaving the newly-built VerticalBox invisible.
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:

	UPROPERTY()
	UVerticalBox* LineContainer = nullptr;

	TArray<FTextData> PendingLines;
	int32 NextPendingIndex = 0;
	FTimerHandle LineRevealTimerHandle;
	FOnAnimationFinished PendingFinishedCallback;

	void RevealNextPendingLine();
};
