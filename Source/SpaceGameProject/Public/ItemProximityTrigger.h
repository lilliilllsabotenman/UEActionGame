// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemParent.h"
#include "PlayerLocationBroadcasterComponent.h"
#include "PlayerLocationResponder.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "ItemProximityTrigger.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEGAMEPROJECT_API UItemProximityTrigger : public UActorComponent, public IPlayerLocationResponder
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TriggerSettings")
	float Distance = 5.f;


	virtual void HandlePlayerLocationUpdated(const FVector& NewLocation) override;

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
