// Fill out your copyright notice in the Description page of Project Settings.


#include "RotationCompositorComponent.h"
#include "RotationEventHubComponent.h"

// Sets default values for this component's properties
URotationCompositorComponent::URotationCompositorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void URotationCompositorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		PlayerQuat = Owner->GetActorQuat();
		AppliedQuat = PlayerQuat;

		if (URotationEventHubComponent* Hub = Owner->FindComponentByClass<URotationEventHubComponent>())
		{
			Hub->GetOnAddPlayerRotation().AddUObject(this, &URotationCompositorComponent::AddQuat);
		}
	}
}

// Called every frame
void URotationCompositorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AppliedQuat = PlayerQuat; // Easing disabled for now; plain assignment.

	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorRotation(AppliedQuat);
	}
}

void URotationCompositorComponent::AddQuat(FQuat newQuat)
{
	// World-space composition: newQuat is expressed in world axes, so it must be applied
	// on the left (PlayerQuat *= newQuat would instead treat it as a local-space delta).
	PlayerQuat = newQuat * PlayerQuat;
}

FQuat URotationCompositorComponent::GetQuat()
{
	return PlayerQuat;
}
