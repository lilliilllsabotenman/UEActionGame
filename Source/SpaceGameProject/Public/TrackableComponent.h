// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrackableComponent.generated.h"

class UTrackerKey;

// 追跡対象になりうることを示す目印コンポーネント。任意のアクターに付けられる。
// GenericObjectTrackerComponentはレベル上のアクターを総なめしてこのKeyを照合し、対象を特定する。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UTrackableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trackable")
	UTrackerKey* Key = nullptr;
};
