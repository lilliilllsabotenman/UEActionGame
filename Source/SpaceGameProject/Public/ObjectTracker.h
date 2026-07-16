// Fill out your copyright notice in the Description page of Project Settings.
// ObjectTracker.h

#pragma once

#include "CoreMinimal.h"
#include "ObjectTracker.generated.h"

class UUserWidget;

// 距離に応じたスケーリングの設定。距離スケーリングが欲しい追跡対象だけがオプトインで使う
USTRUCT(BlueprintType)
struct FDistanceScaleSettings
{
	GENERATED_BODY()

	// この距離以下ではマーカーを最大サイズにする
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ObjectTracker")
	float MinDistance = 500.0f;

	// この距離以上ではマーカーを最小サイズにする
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ObjectTracker")
	float MaxDistance = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ObjectTracker")
	float MinScale = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ObjectTracker")
	float MaxScale = 1.5f;
};

// 追跡対象ごとのマーカーWidget・スクリーン座標を保持する汎用の描画マネージャー。
// Characterが1個所有し、ItemTrackerComponentなど複数のコンポーネントに配って使い回す想定。
// キーはUObject*なので、追跡対象がAActorである必要はない(座標は呼び出し側が渡す)。
UCLASS()
class SPACEGAMEPROJECT_API UObjectTracker : public UObject
{
	GENERATED_BODY()

public:
	// Keyに対応するマーカーWidgetを生成する。既に登録済みなら何もしない
	// UObjectTrackerは素のUObjectでGetWorld()のOuterチェーン解決に頼れないため、Worldは呼び出し側から渡す
	void RegisterTarget(UObject* Key, TSubclassOf<UUserWidget> MarkerWidgetClass, UWorld* World);

	// Keyの登録を解除し、マーカーWidgetをビューポートから外す
	void UnregisterTarget(UObject* Key);

	// WorldLocationをスクリーンに投影してマーカー位置・表示/非表示を更新する
	void UpdatePosition(UObject* Key, const FVector& WorldLocation, UWorld* World);

	// 距離に応じてマーカーの大きさを変える。距離スケーリングが不要な追跡対象は呼ばなくてよい
	void ApplyDistanceScale(UObject* Key, const FVector& WorldLocation, const FDistanceScaleSettings& ScaleSettings, UWorld* World);

private:
	UPROPERTY()
	TMap<UObject*, UUserWidget*> MarkerWidgets;

	UPROPERTY()
	TMap<UObject*, FVector2D> ScreenPositions;
};