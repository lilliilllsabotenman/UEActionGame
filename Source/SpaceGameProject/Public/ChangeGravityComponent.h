// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyMovementComponent.h"
#include "RotationEventHub.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChangeGravityComponent.generated.h"

class AMyCharacter;

DECLARE_MULTICAST_DELEGATE(FOnChangeGravityEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UChangeGravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UChangeGravityComponent();

	FOnChangeGravityEvent OnChangeGravityEvent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY()
	TMap<AActor*, FVector> GravityObjectMap;

	UPROPERTY()
	UMyMovementComponent* MovementComponent = nullptr;

	UPROPERTY()
	TScriptInterface<IRotationEventHub> RotationHub;

	UPROPERTY()
	AMyCharacter* OwnerCharacter = nullptr;

	FRotator PlayerRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Gravity", meta = (ClampMin = "0.0"))
	float GravityConvergenceAngleThreshold = 1.0f;

	// Half-angle of the cone used to sample surrounding surface normals around an impact point.
	UPROPERTY(EditAnywhere, Category = "Gravity", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float ImpactSampleAngle = 15.0f;

	bool bIsConvergingGravity = false;
	

private:

	void SetGravityDirection(FVector newGravityDirection);
	void PlayerGravitySolver();

	FRotator CalculateUprightRotation(FVector Direction);
	FVector GetCurrentGravity() const;

	void HandleCharacterLanded(const FHitResult& Hit);
	void HandleCharacterHit(const FHitResult& Hit, const FVector& Velocity);
	FVector GetAverageImpactNormal(const FHitResult& Hit);
	void ResourceEnpty();
};
