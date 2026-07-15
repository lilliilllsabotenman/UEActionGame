#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterComponent.h"
#include "InputActionValue.h"
#include "RotationEventHub.h"
#include "MyMovementComponent.generated.h"

class UCameraComponent;
class UEnhancedInputComponent;
class UInputAction;
class URopeSolverComponent;

UCLASS()
class SPACEGAMEPROJECT_API
UMyMovementComponent : public UCharacterMovementComponent, public ICharacterComponent
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction=nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction=nullptr;

    UFUNCTION(BlueprintPure, Category = "Movement")
    FVector GetPlayerLocalVelocity() const;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float JumpForce = 600.f;

private :

    FVector CharacterDirection = FVector::DownVector;

    FRotator FixedRot = FRotator::ZeroRotator;

    UCameraComponent* Camera = nullptr;

    UPROPERTY()
    TScriptInterface<IRotationEventHub> RotationHub;

    UPROPERTY()
    URopeSolverComponent* RopeSolverComp = nullptr;

    void PlayerRotationSolver();

public:

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void BindInput(UEnhancedInputComponent* EnhancedInput) override;

    void SetRotation(FVector NewDirection);

    FVector GetCurrentGravity() const;

    void Move(const FInputActionValue& Value);
    void RequestJump();
};

