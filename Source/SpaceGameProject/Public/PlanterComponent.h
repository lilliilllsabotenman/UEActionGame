// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "PlantObject.h"


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"



enum class PlanterState
{
	Planting,
	Free
};

#include "PlanterComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectPlanted, AActor*, PlantedActor);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UPlanterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlanterComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CroneObject")
	TSubclassOf<AActor> plantObject;

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	FOnObjectPlanted OnObjectPlanted;

	APlantObject* CroneObject = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	FVector objectLocatrion = FVector::ZeroVector;
	FRotator objectRotation = FRotator::ZeroRotator;

	PlanterState _state = PlanterState::Free;

	float PlantPointOffset = 0.f;

	void Planting();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SpawnObject();

	template<typename T>
	void ObjectLogationSettings(T* Component, const FVector& Start, const FVector& End);

	void ObjectLocationUpdate();
	void PositionOffsetInitialized();
	void PlantCancel();
};