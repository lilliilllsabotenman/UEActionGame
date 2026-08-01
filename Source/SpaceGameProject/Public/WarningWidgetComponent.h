// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WarningWidgetComponent.generated.h"

class UBlinkTextWidget;

// 画面中央に警告文を点滅表示するBlinkTextWidgetを生成・制御する。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UWarningWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UWarningWidgetComponent();

	// 警告表示を開始する。表示中に呼ばれた場合は文字だけ即時更新し、点滅は継続する。
	UFUNCTION(BlueprintCallable, Category = "Warning")
	void StartWarning(const FString& Content);

	// 警告表示(点滅)を止める。
	UFUNCTION(BlueprintCallable, Category = "Warning")
	void StopWarning();

protected:

	UPROPERTY(EditAnywhere, Category = "Warning")
	TSubclassOf<UBlinkTextWidget> WarningWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Warning")
	float BlinkInterval = 0.3f;

private:

	UPROPERTY()
	UBlinkTextWidget* WidgetInstance = nullptr;

	bool bIsWarningActive = false;
};
