// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemKey.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemObjectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UItemObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemKey")
	UItemKey* Key;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);



public:	
	// Sets default values for this component's properties
	UItemObjectComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	

};
