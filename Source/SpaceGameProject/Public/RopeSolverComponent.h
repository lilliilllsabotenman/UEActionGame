// Fill out your copyright notice in the Description page of Project Settings.
// RopeSolverComponent.h

#pragma once

#include "CharacterComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RopeSetting.h"
#include "RopeSolverComponent.generated.h"

DECLARE_DELEGATE(ReleaseDelegate)

class UCameraComponent;
class UInputAction;
class UEnhancedInputComponent;
class URotationCompositorComponent;
class UCableComponent;
class UMaterialInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API URopeSolverComponent : public UActorComponent, public ICharacterComponent
{
	GENERATED_BODY()

private:



	FVector TargetPosition = FVector::ZeroVector;
	FVector TargetVelocity = FVector::ZeroVector;

	FVector SolvedPlayerVelocity = FVector::ZeroVector;
	FVector SolvedTargetVelocity = FVector::ZeroVector;

	FQuat SolvedLeanRotation = FQuat::Identity;

	bool bIsHooked = false;
	bool bAncorIsMovement = false;

	UPROPERTY()
	UCameraComponent* Camera = nullptr;

	UPROPERTY()
	URotationCompositorComponent* RotationCompositor = nullptr;

	UPROPERTY()
	UCableComponent* CableVisual = nullptr;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope")
	FRopeSettings Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope")
	bool bIsPulling = false;

//=======Cable Visual============

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Visual")
	float CableWidth = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Visual")
	int32 CableNumSegments = 10;

	// Lower values pull the cable taut; higher values let it sag under gravity.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Visual", meta = (ClampMin = "0.0"))
	float CableGravityScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Visual", meta = (ClampMin = "1", ClampMax = "16"))
	int32 CableSolverIterations = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Visual")
	bool bCableEnableStiffness = true;

	// Material applied to the cable mesh; controls its color/look.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Visual")
	UMaterialInterface* CableMaterial = nullptr;

//=======Input============

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Hook = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Reel = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* Release = nullptr;
	


	ReleaseDelegate releaseDelegate;

	// Sets default values for this component's properties
	URopeSolverComponent();

	void SetForce(bool pulling, float DeltaTime);

	// Traces from the owner in TraceDirection (up to Settings.RopeLength) and hooks onto the first hit.
	// Returns false if nothing was hit.
	bool TryHook(FVector TraceDirection);

	void ReelAction();

	void ReleaseHook();

	bool IsHooked() const;

	FQuat GetLeanRotation() const;

	virtual void OnActionTrigger() override;
	virtual void BindInput(UEnhancedInputComponent* EnhancedInput) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
