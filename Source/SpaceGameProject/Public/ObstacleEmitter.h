// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleActor.h"
#include "ObstacleEmitter.generated.h"

UCLASS()
class SPACEGAMEPROJECT_API AObstacleEmitter : public AActor
{
	GENERATED_BODY()

public:
	AObstacleEmitter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	TSubclassOf<AObstacleActor> ObstacleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	int32 PelletCount = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float SpreadHalfAngle = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float LaunchSpeedMin = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float LaunchSpeedMax = 800.f;

	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	void Fire();

protected:
	virtual void BeginPlay() override;
};
