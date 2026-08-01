// Fill out your copyright notice in the Description page of Project Settings.
// RopeSolverComponent.cpp

#include "RopeSolverComponent.h"

#include "ChangeGravityComponent.h"
#include "MyCharacter.h"
#include "ObjectTracker.h"
#include "HookGuideFinder.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "RopeSolver.h"
#include "CableComponent.h"
#include "Materials/MaterialInterface.h"
#include "RotationCompositorComponent.h"
#include "RotationEventHub.h"
#include "HookActionLoggerComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
URopeSolverComponent::URopeSolverComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    PrimaryComponentTick.bCanEverTick = true;

}

void URopeSolverComponent::BeginPlay()
{
    Super::BeginPlay();

    MaxRopeResource = RopeResource;

    if (AActor* Owner = GetOwner())
    {
        Camera = Owner->FindComponentByClass<UCameraComponent>();
        RotationCompositor = Owner->FindComponentByClass<URotationCompositorComponent>();
        HookLogger = Owner->FindComponentByClass<UHookActionLoggerComponent>();

        if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(Owner))
        {
            Tracker = OwnerCharacter->GetObjectTracker();
            OwnerCharacter->OnGetItem.AddUObject(this, &URopeSolverComponent::AddResource);
        }

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
    
    TrySwing();

    if (!bIsHooked) return;

    SetForce(bIsPulling, DeltaTime);

    if (Tracker)
    {
        Tracker->UpdatePosition(this, TargetPosition, GetWorld());
    }

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
        EnhancedInput->BindAction(Hook, ETriggerEvent::Started, this, &URopeSolverComponent::HookAction);
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

void URopeSolverComponent::HookAction()
{
    if (!Camera) return;

    TryHook(Camera->GetForwardVector());
}

bool URopeSolverComponent::IsCanHook()
{
    AActor* Owner = GetOwner();
    if (!Owner || !Camera) return false;

    FVector UnusedPosition;
    return HookGuideFinder::FindHookTarget(GetWorld(), Owner->GetActorLocation(), Camera->GetForwardVector(), Owner, Settings, UnusedPosition);
}

bool URopeSolverComponent::TryHook(FVector TraceDirection)
{
    if (RopeResource <= 0.f) return false;

    AActor* Owner = GetOwner();
    if (!Owner) return false;

    const FVector TraceStart = Owner->GetActorLocation();

    FVector HitPosition;
    if (!HookGuideFinder::FindHookTarget(GetWorld(), TraceStart, TraceDirection, Owner, Settings, HitPosition))
    {
        return false;
    }

    TargetPosition = HitPosition;

    Settings.RopeLength = FVector::Dist(TraceStart, TargetPosition);

    bIsHooked = true;
    AngularVelocity = FVector::ZeroVector;

    if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(Owner))
    {
        OwnerCharacter->SetPlayerRopeState(PlayerRopeState::Rope);
    }

    if (RotationCompositor)
    {
        RotationCompositor->Activate();
    }

    if (CableVisual)
    {
        CableVisual->CableLength = Settings.RopeLength;
        CableVisual->EndLocation = CableVisual->GetComponentTransform().InverseTransformPosition(TargetPosition);
        CableVisual->SetVisibility(true);
    }

    if (Tracker)
    {
        Tracker->RegisterTarget(this, ReticleWidgetClass, GetWorld());
    }

    if (HookLogger)
    {
        HookLogger->LogHookAction(TargetPosition);
    }

    if (HookSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HookSound, Owner->GetActorLocation());
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
    AngularVelocity = FVector::ZeroVector;

    bIsHooked = false;
    bAncorIsMovement = false;

    if (AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwner()))
    {
        const bool bIsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
        OwnerCharacter->SetPlayerRopeState(bIsFalling ? PlayerRopeState::Fall : PlayerRopeState::Ground);
    }

    if (RotationCompositor)
    {
        RotationCompositor->Deactivate();
    }

    if (CableVisual)
    {
        CableVisual->SetVisibility(false);
    }

    if (Tracker)
    {
        Tracker->UnregisterTarget(this);
    }

    if (ReleaseSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ReleaseSound, GetOwner()->GetActorLocation());
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
        const FVector Force = RopeSolver::SolveForce(
            CurrentPlayerPosition,
            CurrentPlayerVelocity,
            TargetPosition,
            Settings
        );

        SolvedPlayerVelocity = Force;
        MoveComp->AddForce(SolvedPlayerVelocity);

        RopeResource = FMath::Max(0.f, RopeResource - RopeCost * Force.Size() * DeltaTime * 0.01f);

        if (RopeResource <= 0.f)
        {
            ReleaseHook();
            return;
        }

        const FVector BankForwardDir = Owner->GetActorForwardVector();
        const FVector GravityUp = -MoveComp->GetGravityDirection();
        const FVector CurrentUp = RotationCompositor ? RotationCompositor->GetQuat().GetUpVector() : Owner->GetActorUpVector();

        AActor* OwnCharacter = GetOwner();
        
        if(OwnCharacter)
        {
             FVector SolveDirection = RopeSolver::SolveDirection(
                OwnCharacter -> GetActorUpVector(),
                (TargetPosition - OwnCharacter -> GetActorLocation()).GetSafeNormal(),
                Force
            );

            if (RotationCompositor)
            {
                FVector newDirection = OwnCharacter -> GetActorUpVector() + (SolveDirection - OwnCharacter -> GetActorUpVector()) / 15;
                GetOwner()->FindComponentByClass<UChangeGravityComponent>() -> SetGravityDirection(-newDirection);
            }
        }



        SolvedTargetVelocity = FVector::ZeroVector;
    }

    if (!bAncorIsMovement) return;
}

void URopeSolverComponent::TrySwing()
{
    FVector Velocity = Cast<AMyCharacter>(GetOwner()) -> GetCharacterMovement() -> Velocity;

    if(bIsHooked)
    {
        _SpeedCache -= Velocity.Size() / MaxSpeed + _SpeedCache / 50;
        SpeedCache = _SpeedCache;
    }

    else
    {
        _SpeedCache -= 0 + _SpeedCache / 20;
        SpeedCache = _SpeedCache;
    }
}

bool URopeSolverComponent::IsHooked() const
{
    return bIsHooked;
}

bool URopeSolverComponent::IsMovingFast() const
{
    const AActor* Owner = GetOwner();
    return Owner && Owner->GetVelocity().Size() >= FastMovementSpeedThreshold;
}

FQuat URopeSolverComponent::GetLeanRotation() const
{
    return SolvedLeanRotation;
}
      
float URopeSolverComponent::GetRopeResource() const
{
    return RopeResource;
}

void URopeSolverComponent::AddResource()
{
    RopeResource = FMath::Min(RopeResource + CostBuffer, MaxRopeResource);
}