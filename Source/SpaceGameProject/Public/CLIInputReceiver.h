// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CLIInputReceiver.generated.h"

// Contract for whatever CLI screen currently wants Up/Down/Confirm input forwarded to it.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UCLIInputReceiver : public UInterface
{
	GENERATED_BODY()
};

class SPACEGAMEPROJECT_API ICLIInputReceiver
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "CLI")
	virtual void NavigateUp() = 0;

	UFUNCTION(BlueprintCallable, Category = "CLI")
	virtual void NavigateDown() = 0;

	UFUNCTION(BlueprintCallable, Category = "CLI")
	virtual void Confirm() = 0;
};
