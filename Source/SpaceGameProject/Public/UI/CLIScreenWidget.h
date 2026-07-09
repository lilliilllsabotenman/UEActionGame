// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/CLILogWidget.h"
#include "UI/CLIMenuWidget.h"
#include "CLIScreenWidget.generated.h"

UENUM(BlueprintType)
enum class ECLIScreenPhase : uint8
{
	Intro,
	Selecting,
	Confirmed
};

/**
 *  CLI-style screen. Acts purely as a mediator between phases:
 *  the log and menu sub-widgets each only report their own completion,
 *  this class decides what happens next.
 */
UCLASS(abstract)
class UCLIScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCLIScreenConfirmed, ECLIMenuOptionKey, SelectedKey);

	UPROPERTY(BlueprintAssignable, Category="CLI")
	FOnCLIScreenConfirmed OnCLIScreenConfirmed;

	UPROPERTY(BlueprintReadOnly, Category="CLI")
	ECLIScreenPhase CurrentPhase = ECLIScreenPhase::Intro;

protected:

	UPROPERTY(meta=(BindWidget))
	UCLILogWidget* LogWidget;

	UPROPERTY(meta=(BindWidget))
	UCLIMenuWidget* MenuWidget;

	virtual void NativeConstruct() override;

private:

	UFUNCTION()
	void HandleLogPlaybackFinished();

	UFUNCTION()
	void HandleSelectionConfirmed(ECLIMenuOptionKey SelectedKey);
};