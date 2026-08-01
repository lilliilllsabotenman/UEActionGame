// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectTracker.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/GameViewportSubsystem.h"

void UObjectTracker::RegisterTarget(UObject* Key, TSubclassOf<UUserWidget> MarkerWidgetClass, UWorld* World)
{
	if (!Key || !MarkerWidgetClass)
	{
		return;
	}
	if (MarkerWidgets.Contains(Key)) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		return;
	}

	if (UUserWidget* Marker = CreateWidget<UUserWidget>(PC, MarkerWidgetClass))
	{
		Marker->AddToViewport();

		// SetPositionInViewportは左上基準、SetRenderScaleは中心基準でズレるため、位置基準もWidget中心に揃える
		// (UE5.7ではAddToViewportはUGameViewportSubsystem管理になっており、UCanvasPanelSlotへはキャストできない)
		if (UGameViewportSubsystem* ViewportSubsystem = UGameViewportSubsystem::Get(World))
		{
			FGameViewportWidgetSlot MarkerSlot = ViewportSubsystem->GetWidgetSlot(Marker);
			MarkerSlot.Alignment = FVector2D(0.5f, 0.5f);
			ViewportSubsystem->SetWidgetSlot(Marker, MarkerSlot);
		}

		MarkerWidgets.Add(Key, Marker);
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