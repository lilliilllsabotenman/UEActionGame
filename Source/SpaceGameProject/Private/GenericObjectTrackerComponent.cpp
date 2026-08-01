// Fill out your copyright notice in the Description page of Project Settings.

#include "GenericObjectTrackerComponent.h"
#include "MyCharacter.h"
#include "TrackerKey.h"
#include "TrackableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceGameProject.h"

UGenericObjectTrackerComponent::UGenericObjectTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

AActor* UGenericObjectTrackerComponent::ResolveTargetActor(const UTrackerKey* TargetKey) const
{
	if (!TargetKey) return nullptr;

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (const UTrackableComponent* Trackable = Actor->FindComponentByClass<UTrackableComponent>())
		{
			if (Trackable->Key == TargetKey)
			{
				return Actor;
			}
		}
	}

	return nullptr;
}

void UGenericObjectTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwner()))
	{
		Tracker = OwnerCharacter->GetObjectTracker();
	}

	TargetActors.SetNum(TrackTargets.Num());
	for (int32 Index = 0; Index < TrackTargets.Num(); ++Index)
	{
		const FObjectTrackerEntry& Entry = TrackTargets[Index];
		AActor* TargetActor = ResolveTargetActor(Entry.TargetKey);
		TargetActors[Index] = TargetActor;

		if (!TargetActor)
		{
			UE_LOG(LogSpaceGameProject, Error, TEXT("GenericObjectTrackerComponent: no TrackableComponent found matching TargetKey"));
			continue;
		}

		if (Tracker)
		{
			Tracker->RegisterTarget(TargetActor, Entry.MarkerWidgetClass, GetWorld());
		}
	}
}

void UGenericObjectTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Tracker) return;

	for (int32 Index = 0; Index < TargetActors.Num(); ++Index)
	{
		AActor* TargetActor = TargetActors[Index];
		if (!TargetActor) continue;

		// 破棄されたActorはIsValidでしか検知できない(GCがポインタをnullにするのはこの後なので、生ポインタのnullチェックでは間に合わない)
		if (!IsValid(TargetActor))
		{
			Tracker->UnregisterTarget(TargetActor);
			TargetActors[Index] = nullptr;
			continue;
		}

		const FVector Location = TargetActor->GetActorLocation();
		Tracker->UpdatePosition(TargetActor, Location, GetWorld());
		Tracker->ApplyDistanceScale(TargetActor, Location, TrackTargets[Index].ScaleSettings, GetWorld());
	}
}
