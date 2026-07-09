// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraComponent.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/Pawn.h"

void UPlayerCameraComponent::BindInput(UEnhancedInputComponent* EnhancedInput)
{
	if (!EnhancedInput || !LookAction) return;

	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &UPlayerCameraComponent::Look);
}

void UPlayerCameraComponent::Look(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();

	if (APawn* Owner = Cast<APawn>(GetOwner()))
	{
		Owner->AddControllerYawInput(Input.X);
		Owner->AddControllerPitchInput(-Input.Y);

	}
}
