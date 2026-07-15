// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemParent.h"
#include "ItemTrackerComponent.generated.h"

class UGameRuleComponent;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UItemTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemTrackerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	// この距離以下ではマーカーを最大サイズにする
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	float MinDistance = 500.0f;

	// この距離以上ではマーカーを最小サイズにする
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	float MaxDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	float MinScale = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemTracker")
	float MaxScale = 1.5f;


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

	UPROPERTY()
	TArray<AItemParent*> ItemObjects;

	UPROPERTY()
	TMap<AItemParent*, FVector2D> ItemScreenPositions;

	UPROPERTY()
	TMap<AItemParent*, UUserWidget*> ItemMarkerWidgets;

	bool bMarkersCreated = false;

	void CreateMarkerWidgets();
	void UpdateItemScreenPositions();
};
