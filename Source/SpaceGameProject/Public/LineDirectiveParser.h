// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class ELineDirectiveParamType : uint8
{
	String,
	Int
};

// One "[Marker:Param]" directive parsed off the front of a line. Param is typed as Int if it's
// fully numeric, String otherwise.
struct FLineDirective
{
	FString Marker;
	ELineDirectiveParamType ParamType = ELineDirectiveParamType::String;
	FString StringParam;
	int32 IntParam = 0;
};

// Splits a single line of the form "[Marker:Param]Content" into its parts. Knows nothing about
// what any given Marker means -- that's left to each caller. No engine/actor dependencies.
class SPACEGAMEPROJECT_API LineDirectiveParser
{
public:

	// Returns false (leaving OutDirective/OutContent untouched) if RawLine does not start with
	// "[" or has no closing "]", i.e. isn't a directive line at all.
	static bool TryParse(const FString& RawLine, FLineDirective& OutDirective, FString& OutContent);
};
