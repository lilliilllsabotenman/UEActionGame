#include "MyMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"
#include "RopeSolverComponent.h"

void UMyMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        Camera = Owner->FindComponentByClass<UCameraComponent>();
        RopeSolverComp = Owner->FindComponentByClass<URopeSolverComponent>();

        if (UActorComponent* HubComponent = Owner->FindComponentByInterface(URotationEventHub::StaticClass()))
        {
            RotationHub = HubComponent;
        }
    }
}

void UMyMovementComponent::BindInput(UEnhancedInputComponent* EnhancedInput)
{
    if (!EnhancedInput) return;

    if (MoveAction)
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UMyMovementComponent::Move);
    }

    if (JumpAction)
    {
        EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &UMyMovementComponent::RequestJump);
    }
}

void UMyMovementComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    const bool bIsRoped = RopeSolverComp && RopeSolverComp->IsHooked();

    // Rope中はRotationEventHub経由の自前回転(PlayerRotationSolver)に任せるため、
    // CharacterMovementComponent標準の移動方向オートフェイスは切っておく。
    bOrientRotationToMovement = !bIsRoped;

    Super::TickComponent(
        DeltaTime,
        TickType,
        ThisTickFunction
    );

     GravityScale += (1 - GravityScale) / 20;

     if (bIsRoped)
     {
         PlayerRotationSolver();
     }
}

void UMyMovementComponent::PlayerRotationSolver()
{
    if (!Camera) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    FVector WorldForward = Camera->GetForwardVector();
    FVector LocalForward = Owner->GetActorTransform().InverseTransformVectorNoScale(WorldForward);

    LocalForward.Normalize();

    float Yaw = FMath::RadiansToDegrees(FMath::Atan2(LocalForward.Y, LocalForward.X));

    FQuat LocalDelta(FVector::UpVector, FMath::DegreesToRadians(Yaw));

    if (RotationHub)
    {
        const FQuat CurrentQuat = Owner->GetActorQuat();
        const FQuat AbsoluteTarget = CurrentQuat * LocalDelta;
        const FQuat WorldDelta = IRotationEventHub::ConvertTargetToDelta(CurrentQuat, AbsoluteTarget);

        RotationHub->AddPlayerRotation(WorldDelta);
    }
}

FVector UMyMovementComponent::GetPlayerLocalVelocity() const
{
    const AActor* Owner = GetOwner();

    if (!Owner) return FVector::ZeroVector;

    return Owner->GetActorTransform().InverseTransformVectorNoScale(Velocity);
}

 FVector UMyMovementComponent::GetCurrentGravity() const
 {
    return GetGravityDirection() * GetWorld()->GetGravityZ() * GravityScale;
 }

 void UMyMovementComponent::Move(const FInputActionValue& Value)
 {
     const FVector2D MoveValue = Value.Get<FVector2D>();

     const APawn* Owner = GetPawnOwner();
     if (!Owner || !Camera) return;

     // 入力方向は常にカメラ基準(現在の重力Up軸に投影)。Actor自身の向きには依存しない。
     const FVector Up = -GetGravityDirection().GetSafeNormal();

     FVector Forward = FVector::VectorPlaneProject(Camera->GetForwardVector(), Up).GetSafeNormal();
     if (Forward.IsNearlyZero())
     {
         Forward = FVector::VectorPlaneProject(Camera->GetUpVector(), Up).GetSafeNormal();
     }

     const FVector Right = FVector::CrossProduct(Up, Forward).GetSafeNormal();

     AddInputVector(Forward * MoveValue.Y);
     AddInputVector(Right * MoveValue.X);
 }

 void UMyMovementComponent::RequestJump()
 {

     SetMovementMode(MOVE_Falling);

     if (const APawn* Owner = GetPawnOwner())
     {
         Velocity += Owner->GetActorUpVector() * JumpForce;
     }
 }
