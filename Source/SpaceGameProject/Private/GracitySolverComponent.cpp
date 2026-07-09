// Fill out your copyright notice in the Description page of Project Settings.


#include "GracitySolverComponent.h"

// Sets default values for this component's properties
UGracitySolverComponent::UGracitySolverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGracitySolverComponent::BeginPlay()
{
	Super::BeginPlay();

	UStaticMeshComponent* Mesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();	

	if (Mesh)
	{
		Mesh->SetEnableGravity(false);
	}
	
}


// Called every frame
void UGracitySolverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

