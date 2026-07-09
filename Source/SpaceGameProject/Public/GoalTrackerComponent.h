// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoalObject.h"
#include "GoalTrackerComponent.generated.h"

class UGameRuleComponent;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UGoalTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGoalTrackerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GoalTracker")
	TSubclassOf<UUserWidget> MarkerWidgetClass;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GoalTracker")
	void OnGoalScreenPositionsUpdated();

private:

	UPROPERTY()
	UGameRuleComponent* GameRule = nullptr;

	UPROPERTY()
	TArray<AGoalObject*> GoalObjects;

	UPROPERTY()
	TMap<AGoalObject*, FVector2D> GoalScreenPositions;

	UPROPERTY()
	TMap<AGoalObject*, UUserWidget*> GoalMarkerWidgets;

	bool bMarkersCreated = false;

	void CreateMarkerWidgets();
	void UpdateGoalScreenPositions();
};