// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RotationCompositorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API URotationCompositorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URotationCompositorComponent();

	// How quickly the applied actor rotation eases toward the composed target rotation.
	UPROPERTY(EditAnywhere, Category = "Rotation", meta = (ClampMin = "0.0"))
	float EaseSpeed = 10.0f;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Activate時、PlayerQuatを現在のActor回転に同期してから制御を握る(再開時のスナップ防止)。
	virtual void Activate(bool bReset = false) override;

	void AddQuat(FQuat newQuat);
	FQuat GetQuat();


private:

	// Target rotation accumulated from all contributors; not reset per frame.
	FQuat PlayerQuat = FQuat::Identity;

	// Rotation actually applied to the actor, eased toward PlayerQuat each tick.
	FQuat AppliedQuat = FQuat::Identity;
};
