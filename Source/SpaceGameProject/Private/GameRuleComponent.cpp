// Fill out your copyright notice in the Description page of Project Settings.


#include "GameRuleComponent.h"
#include "Engine/Engine.h"
#include <stdio.h>

// Sets default values for this component's properties
UGameRuleComponent::UGameRuleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGameRuleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoalObject::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AGoalObject* MyActor = Cast<AGoalObject>(Actor);
		if (MyActor != nullptr)
		{
			GoalObjects.Add(MyActor);
		}
	}
	
}


// Called every frame
void UGameRuleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UGameRuleComponent::MissonCompleted()
{
	for (AGoalObject* Actor : GoalObjects)
	{
		const bool bCompleted = Actor->IsCompleted();
		if (!bCompleted) return false;
	}
	return true;
}