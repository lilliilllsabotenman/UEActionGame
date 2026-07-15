// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemParent.h"
#include "Engine/Engine.h"
#include "MyCharacter.h"

// Sets default values
AItemParent::AItemParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemParent::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AItemParent::OnOverlapBegin);

	PrecomputeBurstSpawnPositions();

}

void AItemParent::PrecomputeBurstSpawnPositions()
{
	BurstSpawnPositions.Reset();
	BurstSpawnPositions.Reserve(BurstSpawnCount);

	for (int32 Index = 0; Index < BurstSpawnCount; ++Index)
	{
		BurstSpawnPositions.Add(GetActorLocation() + FMath::VRand() * BurstSpawnDistance);
	}
}

// Called every frame
void AItemParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemParent::TriggerItemBurst()
{
	if (bIsCompleted || !ItemObjectClass) return;

	for (const FVector& SpawnLocation : BurstSpawnPositions)
	{
		GetWorld()->SpawnActor<AItemObjectActor>(ItemObjectClass, SpawnLocation, GetActorRotation());
	}

	this -> Destroy();
}

void AItemParent::SpawnItemObject()
{
	if(bIsCompleted) return;

	const FVector SpawnLocation = FindValidSpawnLocation();

	AItemObjectActor* SpawnedItem = GetWorld()->SpawnActor<AItemObjectActor>(ItemObjectClass, SpawnLocation, GetActorRotation());
	if (!SpawnedItem) return;

	const float Speed = FMath::RandRange(ItemLaunchSpeedMin, ItemLaunchSpeedMax);
	SpawnedItem->LaunchWithVelocity(GetRandomUpperHemisphereVector() * Speed);
}

FVector AItemParent::FindValidSpawnLocation() const
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

FVector AItemParent::GetRandomUpperHemisphereVector() const
{
	const FVector Up = GetActorUpVector();
	return FMath::VRandCone(Up, FMath::DegreesToRadians(LaunchConeHalfAngle));
}

void AItemParent::Completed()
{
	bIsCompleted = true;
	GetWorldTimerManager().ClearTimer(ItemSpawnTimerHandle);
}

bool AItemParent::IsCompleted()
{
	return bIsCompleted;
}

void AItemParent::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

	AMyCharacter* Character = Cast<AMyCharacter>(OtherActor);

	if(!Character) return;
	bIsCompleted = true;
    Character->Goal();
}
