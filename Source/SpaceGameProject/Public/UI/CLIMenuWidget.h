// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputActionValue.h"
#include "CLIMenuWidget.generated.h"

class UTextBlock;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ECLIMenuOptionKey : uint8
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
class UCLIMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionConfirmed, ECLIMenuOptionKey, SelectedKey);

	UPROPERTY(BlueprintAssignable, Category="CLI")
	FOnSelectionConfirmed OnSelectionConfirmed;

protected:

	/** Text rows for each option, in display order */
	UPROPERTY(EditAnywhere, Category="CLI")
	TArray<UTextBlock*> OptionLabels;

	/** Key for each option, same order as OptionLabels */
	UPROPERTY(EditAnywhere, Category="CLI")
	TArray<ECLIMenuOptionKey> OptionKeys;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* MenuMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* MoveUpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* MoveDownAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* ConfirmAction;

	UPROPERTY(EditAnywhere, Category="CLI")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category="CLI")
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