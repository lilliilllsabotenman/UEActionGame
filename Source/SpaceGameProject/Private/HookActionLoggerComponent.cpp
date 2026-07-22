// Fill out your copyright notice in the Description page of Project Settings.
// HookActionLoggerComponent.cpp

#include "HookActionLoggerComponent.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UHookActionLoggerComponent::UHookActionLoggerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FString UHookActionLoggerComponent::GetLogFilePath()
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("HookActionLog"), TEXT("HookActions.jsonl"));
}

void UHookActionLoggerComponent::LogHookAction(const FVector& Position)
{
#if !UE_BUILD_SHIPPING
	++ActionOrder;

	const FString Line = FString::Printf(
		TEXT("{\"order\":%d,\"x\":%f,\"y\":%f,\"z\":%f}") LINE_TERMINATOR,
		ActionOrder, Position.X, Position.Y, Position.Z);

	FFileHelper::SaveStringToFile(
		Line,
		*GetLogFilePath(),
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		FILEWRITE_Append);
#endif
}
