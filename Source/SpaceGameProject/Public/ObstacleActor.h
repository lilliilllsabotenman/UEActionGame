// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleActor.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class SPACEGAMEPROJECT_API AObstacleActor : public AActor
{
	GENERATED_BODY()

public:
	AObstacleActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle")
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle")
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float LifeSpanSeconds = 5.f;

	void Launch(const FVector& InVelocity);

protected:
	virtual void BeginPlay() override;
};
