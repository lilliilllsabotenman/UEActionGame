// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RotationEventHub.h"
#include "RotationEventHubComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API URotationEventHubComponent : public UActorComponent, public IRotationEventHub
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URotationEventHubComponent();

	virtual void AddPlayerRotation(FQuat Delta) override;

	// Concrete-only: not part of IRotationEventHub. Only the listener (RotationCompositorComponent)
	// needs this; publishers only ever see AddPlayerRotation through the interface.
	FOnAddPlayerRotation& GetOnAddPlayerRotation();

private:

	FOnAddPlayerRotation OnAddPlayerRotation;
};
