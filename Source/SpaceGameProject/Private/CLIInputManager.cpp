// Fill out your copyright notice in the Description page of Project Settings.

#include "CLIInputManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "InputAction.h"

ACLIInputManager::ACLIInputManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACLIInputManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}

	EnableInput(PC);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(MoveUpAction, ETriggerEvent::Started, this, &ACLIInputManager::HandleMoveUp);
		EnhancedInput->BindAction(MoveDownAction, ETriggerEvent::Started, this, &ACLIInputManager::HandleMoveDown);
		EnhancedInput->BindAction(ConfirmAction, ETriggerEvent::Started, this, &ACLIInputManager::HandleConfirm);
	}
}

void ACLIInputManager::SetCurrentReceiver(TScriptInterface<ICLIInputReceiver> NewReceiver)
{
	CurrentReceiver = NewReceiver;
}

void ACLIInputManager::HandleMoveUp(const FInputActionValue& Value)
{
	if (CurrentReceiver)
	{
		CurrentReceiver->NavigateUp();
	}
}

void ACLIInputManager::HandleMoveDown(const FInputActionValue& Value)
{
	if (CurrentReceiver)
	{
		CurrentReceiver->NavigateDown();
	}
}

void ACLIInputManager::HandleConfirm(const FInputActionValue& Value)
{
	if (CurrentReceiver)
	{
		CurrentReceiver->Confirm();
	}
}
