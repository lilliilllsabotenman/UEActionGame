// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

enum class PlantObjectState
{
	Controll,
	Plant
};


#include "PlantObject.generated.h"

UCLASS()
class SPACEGAMEPROJECT_API APlantObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlantObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	PlantObjectState _state = PlantObjectState::Controll;

public:

	void SetPlantLocation(FVector position);
	void Plant();
	FVector GetDownVector();
	void Destroy();
	

};

