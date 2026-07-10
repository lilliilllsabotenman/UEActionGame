// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputActionValue.h"
#include "SelectionMenuWidget.generated.h"

class UTextBlock;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ESelectionOptionKey : uint8
{
	Easy,
	Normal,
	Hard
};

/**
 *  CLI-style selection menu.
 *  Responsible for option display, highlight, input handling and confirm judgement only.
 */
UCLASS(abstract)
class SPACEGAMEPROJECT_API USelectionMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionConfirmed, ESelectionOptionKey, SelectedKey);

	UPROPERTY(BlueprintAssignable, Category = "Selection")
	FOnSelectionConfirmed OnSelectionConfirmed;

protected:

	/** Text rows for each option, in display order */
	UPROPERTY(EditAnywhere, Category = "Selection")
	TArray<UTextBlock*> OptionLabels;

	/** Key for each option, same order as OptionLabels */
	UPROPERTY(EditAnywhere, Category = "Selection")
	TArray<ESelectionOptionKey> OptionKeys;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* MenuMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveDownAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ConfirmAction;

	UPROPERTY(EditAnywhere, Category = "Selection")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Selection")
	FLinearColor HighlightColor = FLinearColor::Green;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:

	int32 SelectedIndex = 0;

	void HandleMoveUp(const FInputActionValue& Value);
	void HandleMoveDown(const FInputActionValue& Value);
	void HandleConfirm(const FInputActionValue& Value);

	void RefreshHighlight();
};
