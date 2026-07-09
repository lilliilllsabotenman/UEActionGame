// OrillenFleeBehavior.cpp
#include "RunAwayItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

URunAwayItemComponent::URunAwayItemComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void URunAwayItemComponent::BeginPlay()
{
    Super::BeginPlay();
    PlayerRef = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void URunAwayItemComponent::TickComponent(
    float DeltaTime, ELevelTick, FActorComponentTickFunction*)
{
    if (!PlayerRef.IsValid()) return;

    const FVector SelfLoc = GetOwner()->GetActorLocation();
    const FVector PlayerLoc = PlayerRef->GetActorLocation();
    const float Dist = FVector::Dist(SelfLoc, PlayerLoc);

    // ヒステリシスで状態遷移
    if (!bFleeing && Dist < DetectRadius)
    {
        bFleeing = true;
    }
    else if (bFleeing && Dist > LeaveRadius)
    {
        bFleeing = false;
    }

    if (!bFleeing) return;

    UpdateRandomOffset(DeltaTime);

    const FVector FleeDir = ComputeFleeDirection();
    const FVector AvoidDir = ComputeWallAvoidance();

    // 3つのベクトルを合成してNormalize
    FVector MoveDir = FleeDir + AvoidDir + RandomOffset * RandomStrength;
    MoveDir = MoveDir.GetSafeNormal();

    const FVector NewLoc = SelfLoc + MoveDir * FleeSpeed * DeltaTime;
    GetOwner()->SetActorLocation(NewLoc);
}

FVector URunAwayItemComponent::ComputeFleeDirection() const
{
    // プレイヤーから離れる方向
    const FVector ToPlayer =
        PlayerRef->GetActorLocation() - GetOwner()->GetActorLocation();
    return (-ToPlayer).GetSafeNormal();
}

FVector URunAwayItemComponent::ComputeWallAvoidance() const
{
    FVector AvoidAccum = FVector::ZeroVector;

    const TArray<FVector> Directions = GetSphereRayDirections();
    const FVector SelfLoc = GetOwner()->GetActorLocation();

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    for (const FVector& Dir : Directions)
    {
        FHitResult Hit;
        const FVector End = SelfLoc + Dir * RayLength;

        if (GetWorld()->LineTraceSingleByChannel(
            Hit, SelfLoc, End, ECC_WorldStatic, Params))
        {
            // ヒットした面の法線方向に押し返す
            // 近いほど強く回避（距離で重み付け）
            const float Weight = 1.f - (Hit.Distance / RayLength);
            AvoidAccum += Hit.Normal * Weight;
        }
    }

    return AvoidAccum.GetSafeNormal();
}

void URunAwayItemComponent::UpdateRandomOffset(float DeltaTime)
{
    RandomTimer -= DeltaTime;
    if (RandomTimer > 0.f) return;

    // 一定間隔でランダム方向を更新
    RandomOffset = FMath::VRand();
    RandomTimer = RandomInterval;
}

TArray<FVector> URunAwayItemComponent::GetSphereRayDirections() const
{
    TArray<FVector> Dirs;
    Dirs.Reserve(RayCount);

    // フィボナッチ球面サンプリング
    const float GoldenRatio = (1.f + FMath::Sqrt(5.f)) / 2.f;
    const float AngleIncrement = PI * 2.f * (2.f - GoldenRatio);

    for (int32 i = 0; i < RayCount; ++i)
    {
        const float T = (float)i / (float)RayCount;
        const float Inclination = FMath::Acos(1.f - 2.f * T);
        const float Azimuth = AngleIncrement * i;

        FVector Dir;
        Dir.X = FMath::Sin(Inclination) * FMath::Cos(Azimuth);
        Dir.Y = FMath::Sin(Inclination) * FMath::Sin(Azimuth);
        Dir.Z = FMath::Cos(Inclination);
        Dirs.Add(Dir);
    }

    return Dirs;
}