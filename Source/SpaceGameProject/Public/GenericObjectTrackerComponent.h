// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectTracker.h"
#include "GenericObjectTrackerComponent.generated.h"

class UUserWidget;
class UObjectTracker;
class UTrackerKey;

// 追跡対象1件分の設定。TrackTargets配列の要素数を増やすことで追跡対象を自由に追加できる
USTRUCT(BlueprintType)
struct FObjectTrackerEntry
{
	GENERATED_BODY()

	// 追跡対象を特定するためのKey。同じKeyをTrackableComponentに設定したアクターが追跡される
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	UTrackerKey* TargetKey = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	FDistanceScaleSettings ScaleSettings;
};

// BPでTrackerKeyアセットを指定し、同じKeyを持つTrackableComponentのアクターを
// 指定Widgetで追従表示する汎用トラッカー。TargetActorはEditInstanceOnlyだとスポーンされる
// キャラクターでは保存先(レベルインスタンス)が無く機能しないため、アセット参照経由で解決する。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UGenericObjectTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGenericObjectTrackerComponent();

	// 追跡対象の設定リスト。要素を追加するだけで追跡対象を増やせる
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	TArray<FObjectTrackerEntry> TrackTargets;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// Character側が所有するインスタンスを受け取って使う(自分では生成しない)
	UPROPERTY()
	UObjectTracker* Tracker = nullptr;

	// BeginPlayでTrackTargetsの各Keyから解決したアクター(TrackTargetsと同じ順序・要素数)
	UPROPERTY()
	TArray<AActor*> TargetActors;

	AActor* ResolveTargetActor(const UTrackerKey* TargetKey) const;
};