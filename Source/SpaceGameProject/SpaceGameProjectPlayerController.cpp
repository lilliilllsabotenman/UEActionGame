// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpaceGameProjectPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "SpaceGameProject.h"
#include "UI/CLIScreenWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ASpaceGameProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogSpaceGameProject, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	// only spawn the CLI screen on local player controllers, and only if a class is configured
	if (IsLocalPlayerController() && CLIScreenWidgetClass)
	{
		CLIScreenWidget = CreateWidget<UCLIScreenWidget>(this, CLIScreenWidgetClass);

		if (CLIScreenWidget)
		{
			CLIScreenWidget->AddToViewport();
		} else {

			UE_LOG(LogSpaceGameProject, Error, TEXT("Could not spawn CLI screen widget."));

		}
	}
}

void ASpaceGameProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool ASpaceGameProjectPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
