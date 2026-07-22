// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HookActionLoggerComponent.generated.h"

// 開発効率化ツール: プレイヤーのフック(Hook)実行位置を連番付きでJSON Lines形式で記録する。
// Shippingビルドでは無効(ログを一切書き出さない)。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UHookActionLoggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHookActionLoggerComponent();

	// フックが成功した位置を記録する。呼ぶたびに連番(Order)を1つ進める。
	void LogHookAction(const FVector& Position);

private:
	int32 ActionOrder = 0;

	// ログの出力先ファイルパスを返す(Saved/HookActionLog/HookActions.jsonl)
	static FString GetLogFilePath();
};
