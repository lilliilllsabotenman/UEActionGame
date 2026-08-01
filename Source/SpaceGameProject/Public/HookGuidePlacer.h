// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookGuidePlacer.generated.h"

class AHookGuide;

// 開発効率化ツール: UHookActionLoggerComponentが記録したフック位置ログ(HookActions.jsonl)を読み込み、
// その座標にHookGuideClassを配置する。StageGeneratorと同じCallInEditorボタン方式。
UCLASS()
class SPACEGAMEPROJECT_API AHookGuidePlacer : public AActor
{
	GENERATED_BODY()

public:
	AHookGuidePlacer();

	// 配置するアクタークラス
	UPROPERTY(EditAnywhere, Category = "HookGuidePlacement")
	TSubclassOf<AHookGuide> HookGuideClass;

	// 再生成時にクリアする対象を追跡する
	UPROPERTY()
	TArray<AActor*> GeneratedActors;

	UFUNCTION(CallInEditor, Category = "HookGuidePlacement")
	void Generate();

	UFUNCTION(CallInEditor, Category = "HookGuidePlacement")
	void ClearGenerated();

	// 蓄積したHookActions.jsonlを削除する(ゴミログが溜まったときに任意のタイミングで実行する)
	UFUNCTION(CallInEditor, Category = "HookGuidePlacement")
	void ClearLog();
};
