// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Reads a text file and splits it into one string per line (blank lines included). No engine/actor dependencies.
class SPACEGAMEPROJECT_API TextFileParser
{
public:

	// FilePath is relative to the project directory (e.g. "Content/Widget/Title/BeginCLI.txt").
	static TArray<FString> ParseLines(const FString& FilePath);
};
