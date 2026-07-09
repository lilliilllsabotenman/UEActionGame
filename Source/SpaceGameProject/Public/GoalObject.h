// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemObjectActor.h"
#include "GoalObject.generated.h"

UCLASS()
class SPACEGAMEPROJECT_API AGoalObject : public AActor
{
	GENERATED_BODY()
	
private:

	bool bIsCompleted = false;

	FTimerHandle ItemSpawnTimerHandle;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void SpawnItemObject();

	FVector GetRandomUpperHemisphereVector() const;
	FVector FindValidSpawnLocation() const;

public:
	// Sets default values for this actor's properties
	AGoalObject();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<AItemObjectActor> ItemObjectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float ItemSpawnInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float ItemLaunchSpeedMin = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float ItemLaunchSpeedMax = 700.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float SpawnSearchRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float LaunchConeHalfAngle = 20.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Completed();
	bool IsCompleted();
};
