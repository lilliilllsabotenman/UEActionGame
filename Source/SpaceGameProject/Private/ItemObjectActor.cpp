// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObjectActor.h"

// Sets default values
AItemObjectActor::AItemObjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// MovementComponent = CreateDefaultSubobject<UItemMovementComponent>(TEXT("ItemMovementComponent"));
}

void AItemObjectActor::LaunchWithVelocity(FVector InVelocity)
{
	if (!MovementComponent) return;
	MovementComponent->SetVelocity(InVelocity);
}

// Called when the game starts or when spawned
void AItemObjectActor::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = FindComponentByClass<UItemMovementComponent>();
}

// Called every frame
void AItemObjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector NewScale = (GetActorScale3D() - FVector(ChangeScaleRate * DeltaTime)).ComponentMax(FVector::ZeroVector);
	SetActorScale3D(NewScale);

	if (IsSizeBelowThreshold())
	{
		Destroy();
	}
}

bool AItemObjectActor::IsSizeBelowThreshold() const
{
	return GetActorScale3D().GetMin() <= MinScaleThreshold;
}

