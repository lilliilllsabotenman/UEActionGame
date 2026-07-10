// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AnimationWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAnimationFinished);

UINTERFACE(MinimalAPI)
class UAnimationWidget : public UInterface
{
	GENERATED_BODY()
};

// Contract for a module that plays out some timed presentation, then reports completion
// through a caller-supplied callback. The caller doesn't know or care what happens inside.
class SPACEGAMEPROJECT_API IAnimationWidget
{
	GENERATED_BODY()

public:
	virtual void DoAnimation(const FOnAnimationFinished& OnFinished) = 0;
};
