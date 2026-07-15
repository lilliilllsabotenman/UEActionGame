// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemObjectActor.h"
#include "ItemParent.generated.h"

UCLASS()
class SPACEGAMEPROJECT_API AItemParent : public AActor
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

	TArray<FVector> BurstSpawnPositions;
	void PrecomputeBurstSpawnPositions();

public:
	// Sets default values for this actor's properties
	AItemParent();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 BurstSpawnCount = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float BurstSpawnDistance = 300.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Completed();
	bool IsCompleted();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void TriggerItemBurst();
};
