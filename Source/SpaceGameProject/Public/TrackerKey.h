// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TrackerKey.generated.h"

// 追跡対象と追跡側を紐付けるためだけのアセット。同じインスタンスをTrackableComponentと
// GenericObjectTrackerComponentの両方に参照させ、参照の一致で対応関係を判定する。
UCLASS(BlueprintType)
class SPACEGAMEPROJECT_API UTrackerKey : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KEY")
	FName KeyID;
};
