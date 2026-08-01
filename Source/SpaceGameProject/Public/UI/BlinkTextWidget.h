// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlinkTextWidget.generated.h"

class UCanvasPanel;
class UTextBlock;

// 画面中央に文字を表示し、ルートウィジェットごとVisibilityを切り替えて点滅させる。
// 点滅の停止条件は呼び出し側(StopBlink)に委ねる自己完結アニメーションではない設計。
UCLASS(abstract)
class SPACEGAMEPROJECT_API UBlinkTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// 表示する文字列を書き換える(点滅状態は変えない)。
	UFUNCTION(BlueprintCallable, Category = "BlinkText")
	void SetBlinkText(const FText& NewText);

	// Intervalごとにルートウィジェットの表示/非表示を交互に切り替え始める。
	UFUNCTION(BlueprintCallable, Category = "BlinkText")
	void StartBlink(float Interval);

	// 点滅を止める。bLeaveVisibleがtrueなら表示状態、falseなら非表示状態で止める。
	UFUNCTION(BlueprintCallable, Category = "BlinkText")
	void StopBlink(bool bLeaveVisible = true);

protected:

	// TextLogWidgetと同様、WidgetTree->RootWidgetをSlateに変換する前にツリーを組み立てる。
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:

	UPROPERTY()
	UCanvasPanel* RootPanel = nullptr;

	UPROPERTY()
	UTextBlock* CenterText = nullptr;

	FTimerHandle BlinkTimerHandle;

	void ToggleBlinkVisibility();
};
