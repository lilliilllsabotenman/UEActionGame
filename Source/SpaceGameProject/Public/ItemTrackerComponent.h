// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemParent.h"
#include "ObjectTracker.h"
#include "ItemTrackerComponent.generated.h"

class UGameRuleComponent;
class UUserWidget;
class UObjectTracker;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UItemTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemTrackerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	FDistanceScaleSettings ScaleSettings;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "ItemTracker")
	void OnItemScreenPositionsUpdated();

private:

	UPROPERTY()
	UGameRuleComponent* GameRule = nullptr;

	// Character側が所有するインスタンスを受け取って使う(自分では生成しない)
	UPROPERTY()
	UObjectTracker* Tracker = nullptr;

	UPROPERTY()
	TArray<AItemParent*> ItemObjects;

	bool bMarkersCreated = false;

	void CreateMarkerWidgets();
	void UpdateItemScreenPositions();
};