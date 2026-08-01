// Fill out your copyright notice in the Description page of Project Settings.
// HookGuidePlacer.cpp

#include "HookGuidePlacer.h"
#include "HookGuide.h"
#include "HookActionLoggerComponent.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SpaceGameProject.h"

AHookGuidePlacer::AHookGuidePlacer()
{
	PrimaryActorTick.bCanEverTick = false;

	HookGuideClass = AHookGuide::StaticClass();
}

void AHookGuidePlacer::Generate()
{
	ClearGenerated();

	UWorld* World = GetWorld();
	if (!World || !HookGuideClass)
	{
		return;
	}

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *UHookActionLoggerComponent::GetLogFilePath()))
	{
		UE_LOG(LogSpaceGameProject, Warning, TEXT("HookGuidePlacer: ログファイルが見つかりません(%s)"), *UHookActionLoggerComponent::GetLogFilePath());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	for (const FString& Line : Lines)
	{
		if (Line.IsEmpty())
		{
			continue;
		}

		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Line);
		if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
		{
			continue;
		}

		FVector Position;
		if (!JsonObject->TryGetNumberField(TEXT("x"), Position.X) ||
			!JsonObject->TryGetNumberField(TEXT("y"), Position.Y) ||
			!JsonObject->TryGetNumberField(TEXT("z"), Position.Z))
		{
			continue;
		}

		if (AActor* Guide = World->SpawnActor<AActor>(HookGuideClass, Position, FRotator::ZeroRotator, SpawnParams))
		{
			GeneratedActors.Add(Guide);
		}
	}

	UE_LOG(LogSpaceGameProject, Log, TEXT("HookGuidePlacer: Placed=%d"), GeneratedActors.Num());
}

void AHookGuidePlacer::ClearGenerated()
{
	for (AActor* Actor : GeneratedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	GeneratedActors.Empty();
}

void AHookGuidePlacer::ClearLog()
{
	ClearGenerated();
	UHookActionLoggerComponent::ClearLog();
}
