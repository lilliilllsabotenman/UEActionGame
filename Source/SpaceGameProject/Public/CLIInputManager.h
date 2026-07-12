// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputActionValue.h"
#include "CLIInputReceiver.h"
#include "CLIInputManager.generated.h"

class UInputMappingContext;
class UInputAction;

// Owns the Up/Down/Confirm EnhancedInput bindings for CLI screens and forwards them to whichever
// ICLIInputReceiver is currently active. Kept off APlayerController on purpose so the controller
// stays free of CLI-specific concerns.
UCLASS()
class SPACEGAMEPROJECT_API ACLIInputManager : public AActor
{
	GENERATED_BODY()

public:

	ACLIInputManager();

	// Screens call this (e.g. on becoming visible) to start receiving Navigate/Confirm calls.
	UFUNCTION(BlueprintCallable, Category = "CLI")
	void SetCurrentReceiver(TScriptInterface<ICLIInputReceiver> NewReceiver);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveDownAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ConfirmAction;

private:

	UPROPERTY()
	TScriptInterface<ICLIInputReceiver> CurrentReceiver;

	void HandleMoveUp(const FInputActionValue& Value);
	void HandleMoveDown(const FInputActionValue& Value);
	void HandleConfirm(const FInputActionValue& Value);
};
