// Fill out your copyright notice in the Description page of Project Settings.
// ScanComponent.cpp

#include "ScanComponent.h"

#include "MyCharacter.h"
#include "ObjectTracker.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

UScanComponent::UScanComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UScanComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(Owner))
		{
			Tracker = OwnerCharacter->GetObjectTracker();
		}

		if (APawn* OwnerPawn = Cast<APawn>(Owner))
		{
			CachedPC = Cast<APlayerController>(OwnerPawn->GetController());
		}
	}

	Slots.SetNum(FMath::Max(SampleCount, 0));
	for (FScanMarkerState& Slot : Slots)
	{
		Slot.MarkerKey = NewObject<UObject>(this);
	}
}

void UScanComponent::BindInput(UEnhancedInputComponent* EnhancedInput)
{
	if (!EnhancedInput || !ScanAction) return;

	EnhancedInput->BindAction(ScanAction, ETriggerEvent::Started, this, &UScanComponent::StartScan);
}

void UScanComponent::StartScan()
{
	bIsScanning = !bIsScanning;

	if (!bIsScanning)
	{
		for (FScanMarkerState& Slot : Slots)
		{
			DeactivateSlot(Slot);
		}
	}
}

void UScanComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsScanning)
	{
		UpdateSampleSlots();
	}
}

void UScanComponent::UpdateSampleSlots()
{
	if (!Tracker || !CachedPC) return;

	int32 SizeX = 0, SizeY = 0;
	CachedPC->GetViewportSize(SizeX, SizeY);
	if (SizeX <= 0 || SizeY <= 0) return;

	const FVector2D ScreenCenter(SizeX * 0.5f, SizeY * 0.5f);
	const float MaxScreenRadius = ScreenCenter.Size();

	for (FScanMarkerState& Slot : Slots)
	{
		// べき指数で半径の抽選を中心寄りに偏らせる(DensityExponentが大きいほど近くが密になる)
		const float Angle = FMath::FRand() * 2.0f * PI;
		const float RadiusAlpha = FMath::Pow(FMath::FRand(), DensityExponent);
		const float Radius = RadiusAlpha * MaxScreenRadius;
		const FVector2D ScreenPos = ScreenCenter + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius;

		FHitResult Hit;
		if (!TraceFromScreenPoint(ScreenPos, Hit) || IsExcluded(Hit.GetActor()) || !Hit.GetComponent())
		{
			DeactivateSlot(Slot);
			continue;
		}

		Tracker->RegisterTarget(Slot.MarkerKey, MarkerWidgetClass, GetWorld());
		Tracker->UpdatePosition(Slot.MarkerKey, Hit.ImpactPoint, GetWorld());
		Tracker->ApplyDistanceScale(Slot.MarkerKey, Hit.ImpactPoint, ScaleSettings, GetWorld());
		Slot.bIsRegistered = true;
	}
}

bool UScanComponent::TraceFromScreenPoint(const FVector2D& ScreenPos, FHitResult& OutHit) const
{
	FVector WorldPos, WorldDir;
	if (!CachedPC->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldPos, WorldDir)) return false;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	const FVector TraceEnd = WorldPos + WorldDir * ScanRange;
	return GetWorld()->LineTraceSingleByChannel(OutHit, WorldPos, TraceEnd, TraceChannel, Params);
}

void UScanComponent::DeactivateSlot(FScanMarkerState& Slot)
{
	if (!Slot.bIsRegistered) return;

	if (Tracker)
	{
		Tracker->UnregisterTarget(Slot.MarkerKey);
	}
	Slot.bIsRegistered = false;
}

bool UScanComponent::IsExcluded(const AActor* Actor) const
{
	if (!Actor) return true;

	for (const TSubclassOf<AActor>& ExcludedClass : ExcludedActorClasses)
	{
		if (ExcludedClass && Actor->IsA(ExcludedClass))
		{
			return true;
		}
	}

	return false;
}
