// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/SoftObjectPath.h"
#include "CLIInputReceiver.h"
#include "SelectionMenuWidget.generated.h"

class UVerticalBox;
class UTextBlock;

enum class ESelectionMenuLineType : uint8
{
	PlainText,
	Item
};

// 1行分のパース結果
struct FSelectionMenuLine
{
	ESelectionMenuLineType Type = ESelectionMenuLineType::PlainText;

	// PlainTextなら行そのまま、Itemならラベル部分のみ
	FString Text;

	// Itemの時だけ有効。CurrentIndexとの照合に使う
	int32 ItemIndex = INDEX_NONE;

	// この行に対応する表示用ウィジェット
	UTextBlock* TextBlock = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionConfirmed, int32, SelectedIndex);

// ItemIndexとレベル名の対応1件分
USTRUCT(BlueprintType)
struct FSelectionLevelEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	int32 Index = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	FString LevelName;
};

// CLI風の選択メニュー。MenuFilePathを1行ずつTextBlock化してVerticalBoxに積む。
// 入力はICLIInputReceiver経由で外部(ACLIInputManager)から届く想定で、自前ではBindしない。
UCLASS(abstract)
class SPACEGAMEPROJECT_API USelectionMenuWidget : public UUserWidget, public ICLIInputReceiver
{
	GENERATED_BODY()

public:

	// 確定時に選ばれたItemIndexを通知
	UPROPERTY(BlueprintAssignable, Category = "Selection")
	FOnSelectionConfirmed OnSelectionConfirmed;

	// CLIInputManagerから呼ばれる。選択を1つ上へ
	UFUNCTION()
	virtual void NavigateUp() override;

	// CLIInputManagerから呼ばれる。選択を1つ下へ
	UFUNCTION()
	virtual void NavigateDown() override;

	// CLIInputManagerから呼ばれる。選択を確定
	UFUNCTION()
	virtual void Confirm() override;

protected:

	// メニュー本文＋[ITEM:N]を書いたtxt
	UPROPERTY(EditAnywhere, Category = "Selection", meta = (RelativeToGameDir, FilePathFilter = "txt"))
	FFilePath MenuFilePath;

	// 確定したItemIndexごとに開くレベル名
	UPROPERTY(EditAnywhere, Category = "Selection")
	TArray<FSelectionLevelEntry> LevelsByIndex;

	UPROPERTY(EditAnywhere, Category = "Selection", meta = (ClampMin = "1"))
	int32 FontSize = 24;

	// 非選択行の色
	UPROPERTY(EditAnywhere, Category = "Selection")
	FLinearColor NormalColor = FLinearColor::White;

	// 選択中行の色
	UPROPERTY(EditAnywhere, Category = "Selection")
	FLinearColor HighlightColor = FLinearColor::Green;

	// TextLogWidgetと同じ理由でRebuildWidget内で構築する
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:

	// 行を積むコンテナ
	UPROPERTY()
	UVerticalBox* LineContainer = nullptr;

	// パース結果。行ごとの表示ウィジェットへの参照も持つ
	TArray<FSelectionMenuLine> ParsedLines;

	// 現在選択中のItemIndex
	int32 CurrentIndex = 0;

	// ParseMenuFileで求めた、存在するItemIndexの最大値
	int32 MaxItemIndex = INDEX_NONE;

	// MenuFilePathを読んでParsedLinesを作る
	void ParseMenuFile();

	// ParsedLinesの各行につきTextBlockを1個生成してLineContainerに積む
	void BuildLineWidgets();

	// CurrentIndexに合わせて各行の色を塗り直す
	void RefreshHighlight();
};
