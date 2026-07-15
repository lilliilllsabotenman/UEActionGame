// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerLocationResponder.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerLocationResponder : public UInterface
{
	GENERATED_BODY()
};

// プレイヤー位置の更新通知を受け取れることを表す抽象。具象クラス(AMyCharacter等)への依存を避けるために使う。
class SPACEGAMEPROJECT_API IPlayerLocationResponder
{
	GENERATED_BODY()

public:
	virtual void HandlePlayerLocationUpdated(const FVector& NewLocation) = 0;
};
