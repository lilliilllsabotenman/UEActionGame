// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeGravityComponent.h"
#include "MyCharacter.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UChangeGravityComponent::UChangeGravityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UChangeGravityComponent::BeginPlay()
{
	Super::BeginPlay();

	MaxGravityResource = GravityResource;

	OwnerCharacter = Cast<AMyCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->OnCharacterLanded.AddUObject(this, &UChangeGravityComponent::HandleCharacterLanded);
		OwnerCharacter->OnCharacterHit.AddUObject(this, &UChangeGravityComponent::HandleCharacterHit);

		MovementComponent = Cast<UMyMovementComponent>(OwnerCharacter->GetCharacterMovement());

		if (UActorComponent* HubComponent = OwnerCharacter->FindComponentByInterface(URotationEventHub::StaticClass()))
		{
			RotationHub = HubComponent;
		}
	}
}

// Called every frame
void UChangeGravityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsConvergingGravity)
    {
        PlayerGravitySolver();
    }
}

void UChangeGravityComponent::AddResource()
{
    GravityResource = FMath::Min(GravityResource + CostBuffer, MaxGravityResource);
}

float UChangeGravityComponent::GetGravityResource() const
{
    return this->GravityResource;
}


void UChangeGravityComponent::SetGravityDirection(FVector newGravityDirection)
{
    if (!MovementComponent) return;

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
        FString::Printf(TEXT("[Apply] SetGravityDirection newGravityDirection=%s"), *newGravityDirection.ToString()));

    const FTransform OwnerTransform = GetOwner()->GetActorTransform();

    FVector TargetUp = -newGravityDirection.GetSafeNormal();
    FVector CurrentUp = OwnerTransform.GetUnitAxis(EAxis::Z);
    FQuat Delta = FQuat::FindBetweenNormals(CurrentUp, TargetUp);

    if (RotationHub)
    {
        RotationHub->AddPlayerRotation(Delta);
    }

    MovementComponent->SetGravityDirection(newGravityDirection);
}

void UChangeGravityComponent::PlayerGravitySolver()
{
    if (!MovementComponent) return;
    const float TraceDistance = 150.f;
    const float SampleOffset = 150.f;
    const float MaxSurfaceAngle = 45.f;
    TArray<FVector> Normals;
    const FVector GravityDir = MovementComponent->GetGravityDirection().GetSafeNormal();
    const FTransform OwnerTransform = GetOwner()->GetActorTransform();
    const FVector Center = OwnerTransform.GetLocation();
    FVector TangentForward = FVector::VectorPlaneProject(OwnerTransform.GetUnitAxis(EAxis::X), GravityDir).GetSafeNormal();
    FVector TangentRight = FVector::CrossProduct(GravityDir, TangentForward).GetSafeNormal();
    TArray<FVector> Samples =
    {
        Center,
        Center + TangentForward * SampleOffset,
        Center - TangentForward * SampleOffset,
        Center + TangentRight * SampleOffset,
        Center - TangentRight * SampleOffset
    };
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    for (const FVector& Sample : Samples)
    {
        FHitResult Hit;
        const FVector Start = Sample - GravityDir * 20.f;
        const FVector End = Start + GravityDir * TraceDistance;
        if (GetOwner()->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
        {
            Normals.Add(Hit.ImpactNormal);
        }
    }
    if (Normals.IsEmpty()) return;
    FVector AverageNormal = FVector::ZeroVector;
    for (const FVector& Normal : Normals)
    {
        AverageNormal += Normal;
    }
    AverageNormal.Normalize();
    const float Dot = FVector::DotProduct(AverageNormal, -GravityDir);
    const float SurfaceAngle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));

    if (SurfaceAngle <= GravityConvergenceAngleThreshold)
    {
        bIsConvergingGravity = false;
        return;
    }

    if (SurfaceAngle > MaxSurfaceAngle) return;
    FVector TargetGravity = (GravityDir - AverageNormal / 30.f).GetSafeNormal();
    MovementComponent->SetGravityDirection(TargetGravity);
}

void UChangeGravityComponent::HandleCharacterLanded(const FHitResult& Hit)
{
    bIsConvergingGravity = true;
}

void UChangeGravityComponent::HandleCharacterHit(const FHitResult& Hit, const FVector& Velocity)
{
    const FVector AverageNormal = GetAverageImpactNormal(Hit);
    const float NormalSpeed = FVector::DotProduct(Velocity, -AverageNormal);

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange,
        FString::Printf(TEXT("[Receive] HandleCharacterHit Velocity=%s NormalSpeed=%.1f"), *Velocity.ToString(), NormalSpeed));

    if (NormalSpeed < StrongImpactSpeedThreshold) return;

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("NormalSpeed Over"));

    
    if (!OwnerCharacter || OwnerCharacter->GetPlayerRopeState() == PlayerRopeState::Rope) return;
    SetGravityDirection(-AverageNormal);
}

FVector UChangeGravityComponent::GetAverageImpactNormal(const FHitResult& Hit)
{
    const float TraceDistance = 150.f;
    const float StartOffset = 20.f;

    const FVector ImpactNormal = Hit.ImpactNormal.GetSafeNormal();
    const FVector Center = Hit.ImpactPoint;

    FVector TangentA = FVector::CrossProduct(ImpactNormal, FVector::UpVector).GetSafeNormal();
    if (TangentA.IsNearlyZero())
    {
        TangentA = FVector::CrossProduct(ImpactNormal, FVector::ForwardVector).GetSafeNormal();
    }
    const FVector TangentB = FVector::CrossProduct(ImpactNormal, TangentA).GetSafeNormal();

    const float SampleOffset = TraceDistance * FMath::Tan(FMath::DegreesToRadians(ImpactSampleAngle));

    const TArray<FVector> Samples =
    {
        Center,
        Center + TangentA * SampleOffset,
        Center - TangentA * SampleOffset,
        Center + TangentB * SampleOffset,
        Center - TangentB * SampleOffset
    };

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    TArray<FVector> Normals;
    for (const FVector& Sample : Samples)
    {
        FHitResult SampleHit;
        const FVector Start = Sample + ImpactNormal * StartOffset;
        const FVector End = Start - ImpactNormal * TraceDistance;
        if (GetOwner()->GetWorld()->LineTraceSingleByChannel(SampleHit, Start, End, ECC_Visibility, Params))
        {
            Normals.Add(SampleHit.ImpactNormal);
        }
    }

    if (Normals.IsEmpty()) return ImpactNormal;

    FVector AverageNormal = FVector::ZeroVector;
    for (const FVector& Normal : Normals)
    {
        AverageNormal += Normal;
    }

    return AverageNormal.GetSafeNormal();
}

void UChangeGravityComponent::ResourceEnpty()
{

}