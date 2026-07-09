// Fill out your copyright notice in the Description page of Project Settings.


#include "InertialLeanComponent.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UInertialLeanComponent::UInertialLeanComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UInertialLeanComponent::BeginPlay()
{
	Super::BeginPlay();
	VelocityHistory.SetNum(SampleCount);
	VelocityHistoryIndex = 0;
	MovementComponent = GetOwner()->FindComponentByClass<UMyMovementComponent>();
	rotationCompositor = GetOwner()->FindComponentByClass<URotationCompositorComponent>();
}

void UInertialLeanComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector CurrentVelocity = MovementComponent->Velocity;
	Add(CurrentVelocity);
	
	FQuat LeanRotation = GetLeanRotation(DeltaTime, MovementComponent->GetCurrentGravity());
	// rotationCompositor->AddQuat(LeanRotation);
}

void UInertialLeanComponent::Add(FVector NewValue)
{
	VelocityHistory[VelocityHistoryIndex] = NewValue;
	VelocityHistoryIndex = (VelocityHistoryIndex + 1) % SampleCount;
}

FVector UInertialLeanComponent::GetAverageVelocity()
{
	FVector Sum = FVector::ZeroVector;
	for (int32 i = 0; i < VelocityHistory.Num(); i++)
	{
		Sum += VelocityHistory[i];
	}
	FVector Average = Sum / SampleCount;
	return Average;
}

FQuat UInertialLeanComponent::CalculateUprightRotation(FVector FeltAccel, FVector Forward)
{
	FVector TargetUp = -FeltAccel.GetSafeNormal();
	FMatrix RotMatrix = FRotationMatrix::MakeFromZX(TargetUp, Forward);
	return RotMatrix.ToQuat();
}

FQuat UInertialLeanComponent::GetLeanRotation(float DeltaTime, FVector GravityAccel)
{
	FVector CurrentAverageVelocity = GetAverageVelocity();
	FVector MeasuredAccel = (CurrentAverageVelocity - PreviousAverageVelocity) / DeltaTime;
	FVector FeltAccel = MeasuredAccel - GravityAccel;

	PreviousAverageVelocity = CurrentAverageVelocity;

	FVector Forward = CurrentAverageVelocity.GetSafeNormal();
	// UE_LOG(LogTemp, Warning, TEXT("Forward: %s, VelocityMag: %f"), *Forward.ToString(), CurrentAverageVelocity.Size());
	return CalculateUprightRotation(FeltAccel, Forward);
}