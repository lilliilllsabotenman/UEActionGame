// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterComponent.generated.h"

class UEnhancedInputComponent;

UINTERFACE(MinimalAPI)
class UCharacterComponent : public UInterface
{
	GENERATED_BODY()
};

// Any component (regardless of its concrete UE base class) can implement this to bind its own
// input and receive the shared per-frame gameplay hooks driven by AMyCharacter.
class SPACEGAMEPROJECT_API ICharacterComponent
{
	GENERATED_BODY()

public:
	virtual void BindInput(UEnhancedInputComponent* EnhancedInput) {}

	virtual void OnActionTrigger() {}
	virtual void OnActionHold() {}
	virtual void OnActionComplete() {}
	virtual void OnActionPassive() {}
};
