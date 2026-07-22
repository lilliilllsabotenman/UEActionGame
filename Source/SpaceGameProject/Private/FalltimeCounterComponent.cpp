// Fill out your copyright notice in the Description page of Project Settings.


#include "FalltimeCounterComponent.h"
#include "MyCharacter.h"

// Sets default values for this component's properties
UFalltimeCounterComponent::UFalltimeCounterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFalltimeCounterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		AddTickPrerequisiteActor(Owner);
	}
}


// Called every frame
void UFalltimeCounterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (OwnerCharacter->GetPlayerRopeState() == PlayerRopeState::Fall ||
		OwnerCharacter->GetPlayerRopeState() == PlayerRopeState::ChangeGravity)
	{
		elapsedTime += DeltaTime;

		if (elapsedTime >= TimeLimit)
		{
			OwnerCharacter->Death();
			elapsedTime = 0.f;
		}
	}
	else
	{
		elapsedTime = 0.f;
	}
}

