#include "MyMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"

void UMyMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        Camera = Owner->FindComponentByClass<UCameraComponent>();

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
    Super::TickComponent(
        DeltaTime,
        TickType,
        ThisTickFunction
    );

     GravityScale += (1 - GravityScale) / 20;

     PlayerRotationSolver();
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
     if (!Owner) return;

     AddInputVector(Owner->GetActorForwardVector() * MoveValue.Y);
     AddInputVector(Owner->GetActorRightVector() * MoveValue.X);
 }

 void UMyMovementComponent::RequestJump()
 {

     SetMovementMode(MOVE_Falling);

     if (const APawn* Owner = GetPawnOwner())
     {
         Velocity += Owner->GetActorUpVector() * JumpForce;
     }
 }