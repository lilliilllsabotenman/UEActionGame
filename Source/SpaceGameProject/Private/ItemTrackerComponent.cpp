// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemTrackerComponent.h"
#include "GameRuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

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
	if (!MarkerWidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	for (AItemParent* Item : ItemObjects)
	{
		if (!Item) continue;

		if (UUserWidget* Marker = CreateWidget<UUserWidget>(PC, MarkerWidgetClass))
		{
			Marker->AddToViewport();
			ItemMarkerWidgets.Add(Item, Marker);
		}
	}
}

void UItemTrackerComponent::UpdateItemScreenPositions()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	for (auto It = ItemMarkerWidgets.CreateIterator(); It; ++It)
	{
		AItemParent* Item = It.Key();
		UUserWidget* Marker = It.Value();

		// 破棄されたItemはIsValidでしか検知できない(GCがポインタをnullにするのはこの後なので、生ポインタのnullチェックでは間に合わない)
		if (!IsValid(Item) || !IsValid(Marker))
		{
			if (Marker)
			{
				Marker->RemoveFromParent();
			}
			ItemScreenPositions.Remove(Item);
			It.RemoveCurrent();
			continue;
		}

		FVector2D ScreenPosition;
		if (UGameplayStatics::ProjectWorldToScreen(PC, Item->GetActorLocation(), ScreenPosition))
		{
			ItemScreenPositions.Add(Item, ScreenPosition);
			Marker->SetPositionInViewport(ScreenPosition);
			Marker->SetVisibility(ESlateVisibility::Visible);

			// 距離が近いほど大きく、遠いほど小さく表示する
			const float Distance = FVector::Dist(PC->PlayerCameraManager->GetCameraLocation(), Item->GetActorLocation());
			const float Alpha = FMath::Clamp(FMath::GetRangePct(MinDistance, MaxDistance, Distance), 0.0f, 1.0f);
			const float Scale = FMath::Lerp(MaxScale, MinScale, Alpha);
			Marker->SetRenderScale(FVector2D(Scale, Scale));
		}
		else
		{
			Marker->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
