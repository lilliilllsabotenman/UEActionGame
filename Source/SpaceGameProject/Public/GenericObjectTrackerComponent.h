// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectTracker.h"
#include "GenericObjectTrackerComponent.generated.h"

class UUserWidget;
class UObjectTracker;
class UTrackerKey;

// BPでTrackerKeyアセットを1つ指定し、同じKeyを持つTrackableComponentのアクターを
// 指定Widgetで追従表示する汎用トラッカー。TargetActorはEditInstanceOnlyだとスポーンされる
// キャラクターでは保存先(レベルインスタンス)が無く機能しないため、アセット参照経由で解決する。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UGenericObjectTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGenericObjectTrackerComponent();

	// 追跡対象を特定するためのKey。同じKeyをTrackableComponentに設定したアクターが追跡される
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	UTrackerKey* TargetKey = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GenericObjectTracker")
	FDistanceScaleSettings ScaleSettings;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// Character側が所有するインスタンスを受け取って使う(自分では生成しない)
	UPROPERTY()
	UObjectTracker* Tracker = nullptr;

	// BeginPlayでTargetKeyから解決したアクター
	UPROPERTY()
	AActor* TargetActor = nullptr;

	AActor* ResolveTargetActor() const;
};
