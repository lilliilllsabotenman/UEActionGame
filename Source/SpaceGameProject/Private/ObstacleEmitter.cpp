// Fill out your copyright notice in the Description page of Project Settings.

#include "ObstacleEmitter.h"

AObstacleEmitter::AObstacleEmitter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AObstacleEmitter::BeginPlay()
{
	Super::BeginPlay();
	Fire();
}

void AObstacleEmitter::Fire()
{
	if (!ObstacleClass) return;

	const FVector Origin = GetActorLocation();
	const FVector FireDirection = GetActorForwardVector();

	for (int32 i = 0; i < PelletCount; ++i)
	{
		AObstacleActor* Obstacle = GetWorld()->SpawnActor<AObstacleActor>(ObstacleClass, Origin, GetActorRotation());
		if (!Obstacle) continue;

		const FVector PelletDirection = FMath::VRandCone(FireDirection, FMath::DegreesToRadians(SpreadHalfAngle));
		const float Speed = FMath::RandRange(LaunchSpeedMin, LaunchSpeedMax);

		Obstacle->Launch(PelletDirection * Speed);
	}
}
