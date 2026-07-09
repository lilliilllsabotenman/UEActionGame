// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalTrackerComponent.h"
#include "GameRuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UGoalTrackerComponent::UGoalTrackerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGoalTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		GameRule = Owner->FindComponentByClass<UGameRuleComponent>();
	}
}


// Called every frame
void UGoalTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!bMarkersCreated)
	{
		if (GameRule)
		{
			GoalObjects = GameRule->GetGoalObjects();
		}

		CreateMarkerWidgets();
		bMarkersCreated = true;
	}

	UpdateGoalScreenPositions();
}

void UGoalTrackerComponent::CreateMarkerWidgets()
{
	if (!MarkerWidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	for (AGoalObject* Goal : GoalObjects)
	{
		if (!Goal) continue;

		if (UUserWidget* Marker = CreateWidget<UUserWidget>(PC, MarkerWidgetClass))
		{
			Marker->AddToViewport();
			GoalMarkerWidgets.Add(Goal, Marker);
		}
	}
}

void UGoalTrackerComponent::UpdateGoalScreenPositions()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	for (AGoalObject* Goal : GoalObjects)
	{
		if (!Goal) continue;

		UUserWidget** MarkerPtr = GoalMarkerWidgets.Find(Goal);
		if (!MarkerPtr || !*MarkerPtr) continue;

		UUserWidget* Marker = *MarkerPtr;

		FVector2D ScreenPosition;
		if (UGameplayStatics::ProjectWorldToScreen(PC, Goal->GetActorLocation(), ScreenPosition))
		{
			GoalScreenPositions.Add(Goal, ScreenPosition);
			Marker->SetPositionInViewport(ScreenPosition);
			Marker->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Marker->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
