// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerLocationBroadcasterComponent.h"

UPlayerLocationBroadcasterComponent::UPlayerLocationBroadcasterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerLocationBroadcasterComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(BroadcastTimerHandle, this, &UPlayerLocationBroadcasterComponent::BroadcastLocation, BroadcastInterval, true);
}

void UPlayerLocationBroadcasterComponent::BroadcastLocation()
{
	if (AActor* Owner = GetOwner())
	{
		OnPlayerLocationUpdated.Broadcast(Owner->GetActorLocation());
	}
}
