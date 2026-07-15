// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerLocationBroadcasterComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerLocationUpdated, const FVector&);

// 付けたActorの位置を一定間隔で通知するだけの汎用コンポーネント。具象クラスへの依存を持たせないための切り出し。
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEGAMEPROJECT_API UPlayerLocationBroadcasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerLocationBroadcasterComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tracking")
	float BroadcastInterval = 0.2f;

	FOnPlayerLocationUpdated OnPlayerLocationUpdated;

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle BroadcastTimerHandle;

	void BroadcastLocation();
};
