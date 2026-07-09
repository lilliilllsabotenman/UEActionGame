// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HookAnchorComponent.generated.h"

// Presence-only marker: an actor with this component is a recommended grapple-hook anchor point.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UHookAnchorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHookAnchorComponent();
};
