// Fill out your copyright notice in the Description page of Project Settings.

#include "HookGuideTrackerComponent.h"
#include "HookGuide.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"

UHookGuideTrackerComponent::UHookGuideTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHookGuideTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwner()))
	{
		Tracker = OwnerCharacter->GetObjectTracker();
	}
}

void UHookGuideTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bMarkersCreated)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHookGuide::StaticClass(), FoundActors);

		HookGuideObjects.Reset();
		for (AActor* Actor : FoundActors)
		{
			if (AHookGuide* Guide = Cast<AHookGuide>(Actor))
			{
				HookGuideObjects.Add(Guide);
			}
		}

		CreateMarkerWidgets();
		bMarkersCreated = true;
	}

	UpdateHookGuideScreenPositions();
}

void UHookGuideTrackerComponent::CreateMarkerWidgets()
{
	if (!Tracker) return;

	for (AHookGuide* Guide : HookGuideObjects)
	{
		if (!Guide) continue;

		Tracker->RegisterTarget(Guide, MarkerWidgetClass, GetWorld());
	}
}

void UHookGuideTrackerComponent::UpdateHookGuideScreenPositions()
{
	if (!Tracker) return;

	for (int32 Index = HookGuideObjects.Num() - 1; Index >= 0; --Index)
	{
		AHookGuide* Guide = HookGuideObjects[Index];

		// 破棄されたGuideはIsValidでしか検知できない(GCがポインタをnullにするのはこの後なので、生ポインタのnullチェックでは間に合わない)
		if (!IsValid(Guide))
		{
			Tracker->UnregisterTarget(Guide);
			HookGuideObjects.RemoveAt(Index);
			continue;
		}

		const FVector Location = Guide->GetActorLocation();
		Tracker->UpdatePosition(Guide, Location, GetWorld());
		Tracker->ApplyDistanceScale(Guide, Location, ScaleSettings, GetWorld());
	}
}
