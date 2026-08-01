// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectTracker.h"
#include "HookGuideTrackerComponent.generated.h"

class UUserWidget;
class UObjectTracker;
class AHookGuide;

// レベル上の全HookGuideをUI上でマーカー追跡する。ItemTrackerComponentと同じ構造で、
// 対象取得だけGameRuleではなくAHookGuideのクラス検索に差し替えたもの。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UHookGuideTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHookGuideTrackerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HookGuideTracker")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HookGuideTracker")
	FDistanceScaleSettings ScaleSettings;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Character側が所有するインスタンスを受け取って使う(自分では生成しない)
	UPROPERTY()
	UObjectTracker* Tracker = nullptr;

	UPROPERTY()
	TArray<AHookGuide*> HookGuideObjects;

	bool bMarkersCreated = false;

	void CreateMarkerWidgets();
	void UpdateHookGuideScreenPositions();
};
