// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemTrackerComponent.h"
#include "GameRuleComponent.h"
#include "MyCharacter.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UItemTrackerComponent::UItemTrackerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UItemTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		GameRule = Owner->FindComponentByClass<UGameRuleComponent>();

		if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(Owner))
		{
			Tracker = OwnerCharacter->GetObjectTracker();
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, Tracker ? FColor::Green : FColor::Red,
			FString::Printf(TEXT("ItemTrackerComponent::BeginPlay: Tracker = %s"), Tracker ? TEXT("valid") : TEXT("NULL")));
	}
}


// Called every frame
void UItemTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!bMarkersCreated)
	{
		if (GameRule)
		{
			ItemObjects = GameRule->GetItemObjects();
		}

		CreateMarkerWidgets();
		bMarkersCreated = true;
	}

	UpdateItemScreenPositions();
}

void UItemTrackerComponent::CreateMarkerWidgets()
{
	if (!Tracker) return;

	for (AItemParent* Item : ItemObjects)
	{
		if (!Item) continue;

		Tracker->RegisterTarget(Item, MarkerWidgetClass, GetWorld());
	}
}

void UItemTrackerComponent::UpdateItemScreenPositions()
{
	if (!Tracker) return;

	for (int32 Index = ItemObjects.Num() - 1; Index >= 0; --Index)
	{
		AItemParent* Item = ItemObjects[Index];

		// 破棄されたItemはIsValidでしか検知できない(GCがポインタをnullにするのはこの後なので、生ポインタのnullチェックでは間に合わない)
		if (!IsValid(Item))
		{
			Tracker->UnregisterTarget(Item);
			ItemObjects.RemoveAt(Index);
			continue;
		}

		const FVector Location = Item->GetActorLocation();
		Tracker->UpdatePosition(Item, Location, GetWorld());
		Tracker->ApplyDistanceScale(Item, Location, ScaleSettings, GetWorld());
	}
}