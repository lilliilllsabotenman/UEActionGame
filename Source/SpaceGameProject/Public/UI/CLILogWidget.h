// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"
#include "CLILogWidget.generated.h"

/**
 *  CLI-style intro log display.
 *  Owns the line-by-line reveal timing - knows nothing about input.
 *  Blueprint is only responsible for how a revealed line is presented visually.
 */
UCLASS(abstract)
class UCLILogWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLogPlaybackFinished);

	UPROPERTY(BlueprintAssignable, Category="CLI")
	FOnLogPlaybackFinished OnLogPlaybackFinished;

protected:

	/** TXT file containing one log line per row, path relative to the project directory */
	UPROPERTY(EditAnywhere, Category="CLI", meta=(RelativeToGameDir, FilePathFilter="txt"))
	FFilePath LogFilePath = { TEXT("Content/Widget/Title/BeginCLI.txt") };

	/** Seconds between each revealed line */
	UPROPERTY(EditAnywhere, Category="CLI")
	float LineInterval = 0.5f;

	/** Exit point: implement in Blueprint to present one revealed line */
	UFUNCTION(BlueprintImplementableEvent, Category="CLI")
	void OnLogLineRevealed(const FString& Line);

	virtual void NativeConstruct() override;

private:

	TArray<FString> LogLines;
	int32 NextLineIndex = 0;
	FTimerHandle LineTimerHandle;

	void RevealNextLine();
};