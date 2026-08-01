// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "CoreMinimal.h"

class SPACEGAMEPROJECT_API PlayerLocationCalculatior
{
public:
	PlayerLocationCalculatior();
	~PlayerLocationCalculatior();

	FVector CalculateBurstCenterUV(const APlayerController* PlayerController, const FVector& WorldPosition);
};
