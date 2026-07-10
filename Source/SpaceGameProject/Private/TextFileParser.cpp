// Fill out your copyright notice in the Description page of Project Settings.

#include "TextFileParser.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

TArray<FString> TextFileParser::ParseLines(const FString& FilePath)
{
	TArray<FString> Lines;
	const FString FullPath = FPaths::Combine(FPaths::ProjectDir(), FilePath);
	FFileHelper::LoadFileToStringArray(Lines, *FullPath);
	return Lines;
}
