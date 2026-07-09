// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/CLILogWidget.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UCLILogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const FString FullPath = FPaths::Combine(FPaths::ProjectDir(), LogFilePath.FilePath);
	FFileHelper::LoadFileToStringArray(LogLines, *FullPath);

	GetWorld()->GetTimerManager().SetTimer(LineTimerHandle, this, &UCLILogWidget::RevealNextLine, LineInterval, true);
}

void UCLILogWidget::RevealNextLine()
{
	OnLogLineRevealed(LogLines[NextLineIndex]);
	++NextLineIndex;

	if (NextLineIndex >= LogLines.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(LineTimerHandle);
		OnLogPlaybackFinished.Broadcast();
	}
}