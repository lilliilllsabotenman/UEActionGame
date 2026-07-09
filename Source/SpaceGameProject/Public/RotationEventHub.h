// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RotationEventHub.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddPlayerRotation, FQuat);

UINTERFACE(MinimalAPI)
class URotationEventHub : public UInterface
{
	GENERATED_BODY()
};

// Shared channel for rotation contributions. Publishers submit a world-space delta without
// knowing who (if anyone) is listening.
class SPACEGAMEPROJECT_API IRotationEventHub
{
	GENERATED_BODY()

public:
	virtual void AddPlayerRotation(FQuat Delta) = 0;

	// Pure helper for publishers that only know their desired absolute orientation, not a delta:
	// converts TargetRotation into a delta relative to CurrentRotation, suitable for AddPlayerRotation.
	static FQuat ConvertTargetToDelta(const FQuat& CurrentRotation, const FQuat& TargetRotation)
	{
		return TargetRotation * CurrentRotation.Inverse();
	}
};
