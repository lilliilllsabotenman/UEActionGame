// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CharacterComponent.h"
#include "InputActionValue.h"
#include "PlayerCameraComponent.generated.h"

class UEnhancedInputComponent;
class UInputAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UPlayerCameraComponent : public UCameraComponent, public ICharacterComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction = nullptr;

	virtual void BindInput(UEnhancedInputComponent* EnhancedInput) override;

	void Look(const FInputActionValue& Value);
};
