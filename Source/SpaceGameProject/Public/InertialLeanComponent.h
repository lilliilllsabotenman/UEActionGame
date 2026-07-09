// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyMovementComponent.h"
#include "RotationCompositorComponent.h"
#include "InertialLeanComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UInertialLeanComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInertialLeanComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SampleCount = 8;

	TArray<FVector> VelocityHistory;
	int32 VelocityHistoryIndex;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// InertialLeanComponent.h
private:
	void Add(FVector NewValue);
	FVector GetAverageVelocity();
	FVector PreviousAverageVelocity;
	FQuat GetLeanRotation(float DeltaTime, FVector GravityAccel);
	FQuat CalculateUprightRotation(FVector FeltAccel, FVector Forward);

	UPROPERTY()
	UMyMovementComponent* MovementComponent;
	URotationCompositorComponent* rotationCompositor;
};
