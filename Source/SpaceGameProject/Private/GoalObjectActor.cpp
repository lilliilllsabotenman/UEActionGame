// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalObjectActor.h"
#include "MyCharacter.h"

// Sets default values
AGoalObjectActor::AGoalObjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoalObjectActor::BeginPlay()
{
	Super::BeginPlay();

	UStaticMeshComponent* BoxComp = FindComponentByClass<UStaticMeshComponent>();
	if (BoxComp)
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AGoalObjectActor::OnOverlapBegin);
	}
}

// Called every frame
void AGoalObjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoalObjectActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AMyCharacter* Player = Cast<AMyCharacter>(OtherActor))
	{

		Player->Goal();
	}
}

