// Fill out your copyright notice in the Description page of Project Settings.
// RopeSolverComponent.cpp

#include "RopeSolverComponent.h"

#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "RopeSolver.h"
#include "DrawDebugHelpers.h"
#include "CableComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values for this component's properties
URopeSolverComponent::URopeSolverComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    PrimaryComponentTick.bCanEverTick = true;

}

void URopeSolverComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        Camera = Owner->FindComponentByClass<UCameraComponent>();
        RotationCompositor = Owner->FindComponentByClass<URotationCompositorComponent>();

        CableVisual = NewObject<UCableComponent>(Owner, TEXT("RopeCableVisual"));
        if (CableVisual)
        {
            CableVisual->SetupAttachment(Owner->GetRootComponent());
            CableVisual->bAttachStart = true;
            CableVisual->bAttachEnd = true;
            CableVisual->CableWidth = CableWidth;
            CableVisual->NumSegments = CableNumSegments;
            CableVisual->CableGravityScale = CableGravityScale;
            CableVisual->SolverIterations = CableSolverIterations;
            CableVisual->bEnableStiffness = bCableEnableStiffness;
            if (CableMaterial)
            {
                CableVisual->SetMaterial(0, CableMaterial);
            }
            CableVisual->SetVisibility(false);
            CableVisual->RegisterComponent();
        }
    }
}

// Called every frame
void URopeSolverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsHooked) return;

    SetForce(bIsPulling, DeltaTime);

    if (CableVisual)
    {
        CableVisual->CableLength = Settings.RopeLength;
        CableVisual->EndLocation = CableVisual->GetComponentTransform().InverseTransformPosition(TargetPosition);
    }
}


void URopeSolverComponent::BindInput(UEnhancedInputComponent* EnhancedInput)
{
    if (!EnhancedInput) return;

    if (Hook)
    {
        EnhancedInput->BindAction(Hook, ETriggerEvent::Started, this, &URopeSolverComponent::OnActionTrigger);
    }

    if(Reel)
    {
        EnhancedInput->BindAction(Reel, ETriggerEvent::Triggered, this, &URopeSolverComponent::ReelAction);
    }

    if (Hook)
    {
        EnhancedInput->BindAction(Hook, ETriggerEvent::Completed, this, &URopeSolverComponent::ReleaseHook);
    }
}

void URopeSolverComponent::OnActionTrigger()
{
    if (!Camera) return;
    TryHook(Camera->GetForwardVector());
}

bool URopeSolverComponent::TryHook(FVector TraceDirection)
{
    AActor* Owner = GetOwner();
    if (!Owner) return false;

    const FVector TraceStart = Owner->GetActorLocation();
    const FVector TraceEnd = TraceStart + TraceDirection.GetSafeNormal() * Settings.MaxRopeLength;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    FHitResult Hit;
    if (!GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
    {
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);
        return false;
    }

    DrawDebugLine(GetWorld(), TraceStart, Hit.ImpactPoint, FColor::Green, false, 2.0f, 0, 1.0f);
    DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.0f, 12, FColor::Green, false, 2.0f);

    TargetPosition = Hit.ImpactPoint;

    Settings.RopeLength = FVector::Dist(TraceStart, Hit.ImpactPoint);

    bIsHooked = true;

    if (CableVisual)
    {
        CableVisual->CableLength = Settings.RopeLength;
        CableVisual->EndLocation = CableVisual->GetComponentTransform().InverseTransformPosition(TargetPosition);
        CableVisual->SetVisibility(true);
    }

    return true;
}

void URopeSolverComponent::ReelAction()
{
    if (Settings.RopeLength <= 0.5f) return;
    Settings.RopeLength -= Settings.ReelSpeed * GetWorld() -> GetDeltaSeconds();
}


void URopeSolverComponent::ReleaseHook()
{
    TargetPosition = FVector::ZeroVector;
    TargetVelocity = FVector::ZeroVector;

    SolvedPlayerVelocity = FVector::ZeroVector;
    SolvedTargetVelocity = FVector::ZeroVector;
    SolvedLeanRotation = FQuat::Identity;

    bIsHooked = false;
    bAncorIsMovement = false;

    if (CableVisual)
    {
        CableVisual->SetVisibility(false);
    }

    releaseDelegate.ExecuteIfBound();
}


//===========ForceSolver================

void URopeSolverComponent::SetForce(bool pulling, float DeltaTime)
{
    if (!bIsHooked) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    ACharacter* OwnerCharacter = Cast<ACharacter>(Owner);
    if (!OwnerCharacter) return;

    UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
    if (!MoveComp) return;

    const FVector CurrentPlayerPosition = Owner->GetActorLocation();
    const FVector CurrentPlayerVelocity = Owner->GetVelocity();

    if (!pulling)
    {
        SolvedPlayerVelocity = RopeSolver::SolveForce(
            CurrentPlayerPosition,
            CurrentPlayerVelocity,
            TargetPosition,
            Settings
        );

        MoveComp->AddForce(SolvedPlayerVelocity);

        SolvedLeanRotation = RopeSolver::SolveRotation(
            SolvedPlayerVelocity,
            Owner->GetActorForwardVector()
        );

        SolvedTargetVelocity = FVector::ZeroVector;
    }

    if (!bAncorIsMovement) return;
}

bool URopeSolverComponent::IsHooked() const
{
    return bIsHooked;
}

FQuat URopeSolverComponent::GetLeanRotation() const
{
    return SolvedLeanRotation;
}