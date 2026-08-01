// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateObserver.generated.h"

enum class PlayerRopeState : uint8;

DECLARE_DELEGATE_RetVal(PlayerRopeState, FGetRopeStateDelegate);

UINTERFACE(MinimalAPI)
class UStateObserver : public UInterface
{
	GENERATED_BODY()
};

// CharacterがGetRopeStateDelegateをバインドし、Componentが任意タイミングでStateをPullするための抽象契約
class SPACEGAMEPROJECT_API IStateObserver
{
	GENERATED_BODY()

public:
	virtual FGetRopeStateDelegate& GetRopeStateDelegate() = 0;
};
