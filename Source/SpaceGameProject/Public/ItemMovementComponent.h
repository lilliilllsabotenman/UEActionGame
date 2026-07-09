// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemMovementComponent.generated.h"

class AMyCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UItemMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemMovementComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float GravityScale = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float VelocityDamping = 0.02f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cohesion")
	float CohesionRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cohesion")
	float CohesionStrength = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAttraction")
	float PlayerAttractionStrength = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	float SquishAmount = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	float SquishDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Landing")
	float StuckGracePeriod = 0.2f;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetVelocity(FVector NewVelocity);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	bool bIsStuck = false;
	float AgeTimer = 0.f;

	bool bIsSquishing = false;
	float SquishTimer = 0.f;
	FVector SquishAxis = FVector::UpVector;
	FVector LastSquishMultiplier = FVector::OneVector;

	TWeakObjectPtr<AMyCharacter> BoundPlayerCharacter;
	FVector LastKnownPlayerLocation = FVector::ZeroVector;
	bool bHasPlayerLocation = false;

	void ApplyCohesion(float DeltaTime);
	void ApplyPlayerAttraction(float DeltaTime);
	void ApplyMovement(float DeltaTime);
	void StartSquish(const FVector& ImpactNormal);
	void UpdateSquish(float DeltaTime);
	void HandlePlayerLocationUpdated(const FVector& NewLocation);
	void EnsurePlayerCharacterBound();
};
