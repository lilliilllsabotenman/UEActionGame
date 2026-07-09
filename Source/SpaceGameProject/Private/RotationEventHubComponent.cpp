// Fill out your copyright notice in the Description page of Project Settings.


#include "RotationEventHubComponent.h"

// Sets default values for this component's properties
URotationEventHubComponent::URotationEventHubComponent()
{
}

void URotationEventHubComponent::AddPlayerRotation(FQuat Delta)
{
	OnAddPlayerRotation.Broadcast(Delta);
}

FOnAddPlayerRotation& URotationEventHubComponent::GetOnAddPlayerRotation()
{
	return OnAddPlayerRotation;
}
