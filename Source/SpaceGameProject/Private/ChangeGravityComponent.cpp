// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeGravityComponent.h"
#include "MyCharacter.h"
#include "AntiGravityFloor.h"

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

void UChangeGravityComponent::SetGravityDirection(FVector newGravityDirection)
{
    if (!MovementComponent) return;

    const FTransform OwnerTransform = GetOwner()->GetActorTransform();

    FVector TargetUp = -newGravityDirection.GetSafeNormal();
    FVector CurrentUp = OwnerTransform.GetUnitAxis(EAxis::Z);
    FQuat Delta = FQuat::FindBetweenNormals(CurrentUp, TargetUp);

    if (RotationHub)
    {
        RotationHub->AddPlayerRotation(Delta);
    }

    // MovementComponent->SetGravityDirection(newGravityDirection);

    // MovementComponent->GravityScale = 1.f;

    if (OwnerCharacter)
    {
        OwnerCharacter->SetWeightless(false);
        OwnerCharacter->SetPlayerRopeState(PlayerRopeState::Ground);
    }
}

void UChangeGravityComponent::PlayerGravitySolver()
{
    FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

    if (CurrentLevelName != TEXT("Hard")) return;
    const float TraceDistance = 150.f;
    const float SampleOffset = 150.f;
    const float MaxSurfaceAngle = 36.f;
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

void UChangeGravityComponent::NotifyAntiGravityFloorEntered(AAntiGravityFloor* Floor)
{
    if (!Floor || !OwnerCharacter) return;

    if (Floor->IsAntiGravity)
    {
        // 無重力化専用の床: まだ無重力でなければ入る(すでに無重力なら何もしない)
        if (!OwnerCharacter->IsWeightless() && MovementComponent)
        {
            OwnerCharacter->SetPlayerRopeState(PlayerRopeState::AntiGravity);
            OwnerCharacter->SetWeightless(true);
            MovementComponent->GravityScale = 0.f;
        }
    }
    else
    {
        // 重力復帰専用の床: 無重力状態のときだけ、床のUp方向(エディタで回転指定)を重力の戻し先にする
        if (OwnerCharacter->IsWeightless())
        {
            SetGravityDirection(-Floor->GetActorUpVector());
        }
    }
}

void UChangeGravityComponent::HandleCharacterHit(const FHitResult& Hit, const FVector& Velocity)
{
    if (!OwnerCharacter) return;

    // ハード難易度のときだけ、接地面の法線に重力方向を合わせる
    const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
    if (CurrentLevelName != TEXT("Hard")) return;

    const FVector AverageNormal = GetAverageImpactNormal(Hit);
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