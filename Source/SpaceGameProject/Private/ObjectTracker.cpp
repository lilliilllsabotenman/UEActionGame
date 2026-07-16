// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectTracker.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

void UObjectTracker::RegisterTarget(UObject* Key, TSubclassOf<UUserWidget> MarkerWidgetClass, UWorld* World)
{
	if (!Key || !MarkerWidgetClass)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ObjectTracker::RegisterTarget: Key or MarkerWidgetClass is null"));
		return;
	}
	if (MarkerWidgets.Contains(Key)) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ObjectTracker::RegisterTarget: PlayerController is null"));
		return;
	}

	if (UUserWidget* Marker = CreateWidget<UUserWidget>(PC, MarkerWidgetClass))
	{
		Marker->AddToViewport();
		MarkerWidgets.Add(Key, Marker);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ObjectTracker::RegisterTarget: Marker created"));
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ObjectTracker::RegisterTarget: CreateWidget failed"));
	}
}

void UObjectTracker::UnregisterTarget(UObject* Key)
{
	if (UUserWidget** Found = MarkerWidgets.Find(Key))
	{
		if (UUserWidget* Marker = *Found)
		{
			Marker->RemoveFromParent();
		}
	}

	MarkerWidgets.Remove(Key);
	ScreenPositions.Remove(Key);
}

void UObjectTracker::UpdatePosition(UObject* Key, const FVector& WorldLocation, UWorld* World)
{
	UUserWidget** Found = MarkerWidgets.Find(Key);
	if (!Found) return;

	UUserWidget* Marker = *Found;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return;

	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(PC, WorldLocation, ScreenPosition))
	{
		ScreenPositions.Add(Key, ScreenPosition);
		Marker->SetPositionInViewport(ScreenPosition);
		Marker->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Marker->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UObjectTracker::ApplyDistanceScale(UObject* Key, const FVector& WorldLocation, const FDistanceScaleSettings& ScaleSettings, UWorld* World)
{
	UUserWidget** Found = MarkerWidgets.Find(Key);
	if (!Found) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC || !PC->PlayerCameraManager) return;

	// 距離が近いほど大きく、遠いほど小さく表示する
	const float Distance = FVector::Dist(PC->PlayerCameraManager->GetCameraLocation(), WorldLocation);
	const float Alpha = FMath::Clamp(FMath::GetRangePct(ScaleSettings.MinDistance, ScaleSettings.MaxDistance, Distance), 0.0f, 1.0f);
	const float Scale = FMath::Lerp(ScaleSettings.MaxScale, ScaleSettings.MinScale, Alpha);
	(*Found)->SetRenderScale(FVector2D(Scale, Scale));
}