// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "ItemParent.h"
#include "Components/ActorComponent.h"
#include "GameRuleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UGameRuleComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TArray<AItemParent*> ItemObjects;


public:	
	// Sets default values for this component's properties
	UGameRuleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	bool MissonCompleted();

	const TArray<AItemParent*>& GetItemObjects() const { return ItemObjects; }
};
	