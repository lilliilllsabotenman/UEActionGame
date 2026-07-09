// Fill out your copyright notice in the Description page of Project Settings.

#include "GoalObject.h"
#include "Engine/Engine.h"
#include "MyCharacter.h"

// Sets default values
AGoalObject::AGoalObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoalObject::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AGoalObject::OnOverlapBegin);

	if (ItemObjectClass)
	{
		GetWorldTimerManager().SetTimer(ItemSpawnTimerHandle, this, &AGoalObject::SpawnItemObject, ItemSpawnInterval, true);
	}
}

void AGoalObject::SpawnItemObject()
{
	if(bIsCompleted) return;

	const FVector SpawnLocation = FindValidSpawnLocation();

	AItemObjectActor* SpawnedItem = GetWorld()->SpawnActor<AItemObjectActor>(ItemObjectClass, SpawnLocation, GetActorRotation());
	if (!SpawnedItem) return;

	const float Speed = FMath::RandRange(ItemLaunchSpeedMin, ItemLaunchSpeedMax);
	SpawnedItem->LaunchWithVelocity(GetRandomUpperHemisphereVector() * Speed);
}

FVector AGoalObject::FindValidSpawnLocation() const
{
	const FVector Up = GetActorUpVector();
	const int32 MaxAttempts = 10;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
	{
		const FVector RandomTangent = FVector::VectorPlaneProject(FMath::VRand(), Up).GetSafeNormal();
		const FVector TraceStart = GetActorLocation() + RandomTangent * FMath::FRandRange(0.f, SpawnSearchRadius) + Up * 200.f;
		const FVector TraceEnd = TraceStart - Up * 1000.f;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			return Hit.ImpactPoint;
		}
	}

	return GetActorLocation();
}

FVector AGoalObject::GetRandomUpperHemisphereVector() const
{
	const FVector Up = GetActorUpVector();
	return FMath::VRandCone(Up, FMath::DegreesToRadians(LaunchConeHalfAngle));
}

// Called every frame
void AGoalObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoalObject::Completed()
{
	bIsCompleted = true;
	GetWorldTimerManager().ClearTimer(ItemSpawnTimerHandle);
}

bool AGoalObject::IsCompleted()
{
	return bIsCompleted;
}

void AGoalObject::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

	AMyCharacter* Character = Cast<AMyCharacter>(OtherActor);

	if(!Character) return;
	bIsCompleted = true;
    Character->Goal();
}
