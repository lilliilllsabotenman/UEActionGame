// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FalltimeCounterComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UFalltimeCounterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFalltimeCounterComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TimeSettings")
	float TimeLimit = 5.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	float elapsedTime = 0.f;

	void FallTimeCount(float dletaTime);
		
};
