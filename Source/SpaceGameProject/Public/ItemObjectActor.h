// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemMovementComponent.h"
#include "ItemObjectActor.generated.h"

UCLASS()
class SPACEGAMEPROJECT_API AItemObjectActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemObjectActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ScaleRate")
	float ChangeScaleRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ScaleRate")
	float MinScaleThreshold = 0.1f;

	UPROPERTY()
	UItemMovementComponent* MovementComponent = nullptr;

	void LaunchWithVelocity(FVector InVelocity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "ScaleRate")
	bool IsSizeBelowThreshold() const;
};
