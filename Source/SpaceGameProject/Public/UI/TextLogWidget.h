// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"
#include "AnimationWidget.h"
#include "TextLogWidget.generated.h"

class UTextBlock;

/**
 *  Self-contained screen-filling text log.
 *  Builds its own full-screen TextBlock and loads LogFilePath into it when DoAnimation is called.
 */
UCLASS(abstract)
class SPACEGAMEPROJECT_API UTextLogWidget : public UUserWidget, public IAnimationWidget
{
	GENERATED_BODY()

public:

	virtual void DoAnimation(const FOnAnimationFinished& OnFinished) override;

	// Appends each line to the log, in order.
	UFUNCTION(BlueprintCallable, Category = "TextLog")
	void DisplayLines(const TArray<FString>& Lines);

protected:

	/** Text file (relative to the project directory) loaded into the log on construct */
	UPROPERTY(EditAnywhere, Category = "TextLog", meta = (RelativeToGameDir, FilePathFilter = "txt"))
	FFilePath LogFilePath;

	// Seconds between each revealed line. 0 reveals every queued line immediately.
	UPROPERTY(EditAnywhere, Category = "TextLog", meta = (ClampMin = "0.0"))
	float LineInterval = 0.3f;

	UPROPERTY(EditAnywhere, Category = "TextLog", meta = (ClampMin = "1"))
	int32 FontSize = 24;

	// Builds the CanvasPanel/TextBlock tree before Super converts WidgetTree->RootWidget to Slate.
	// Doing this in NativeConstruct instead is too late: RebuildWidget runs first and would ship
	// the old (empty) root to the screen, leaving the newly-built TextBlock invisible.
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:

	UPROPERTY()
	UTextBlock* TextBlock = nullptr;

	TArray<FString> PendingLines;
	int32 NextPendingIndex = 0;
	FTimerHandle LineRevealTimerHandle;
	FOnAnimationFinished PendingFinishedCallback;

	void RevealNextPendingLine();
};
