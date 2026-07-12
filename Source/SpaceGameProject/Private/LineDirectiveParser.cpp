// Fill out your copyright notice in the Description page of Project Settings.

#include "LineDirectiveParser.h"

bool LineDirectiveParser::TryParse(const FString& RawLine, FLineDirective& OutDirective, FString& OutContent)
{
	if (!RawLine.StartsWith(TEXT("[")))
	{
		return false;
	}

	int32 CloseBracketIndex;
	if (!RawLine.FindChar(TEXT(']'), CloseBracketIndex))
	{
		return false;
	}

	const FString Body = RawLine.Mid(1, CloseBracketIndex - 1);

	FString Marker = Body;
	FString Param;
	Body.Split(TEXT(":"), &Marker, &Param);

	FLineDirective Directive;
	Directive.Marker = Marker;

	if (!Param.IsEmpty() && FCString::IsNumeric(*Param))
	{
		Directive.ParamType = ELineDirectiveParamType::Int;
		Directive.IntParam = FCString::Atoi(*Param);
	}
	else
	{
		Directive.ParamType = ELineDirectiveParamType::String;
		Directive.StringParam = Param;
	}

	OutDirective = Directive;
	OutContent = RawLine.Mid(CloseBracketIndex + 1);

	return true;
}