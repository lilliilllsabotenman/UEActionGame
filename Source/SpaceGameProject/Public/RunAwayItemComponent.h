// OrillenFleeBehavior.h
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RunAwayItemComponent.generated.h"

UCLASS(ClassGroup = (Orilen), meta = (BlueprintSpawnableComponent))
class SPACEGAMEPROJECT_API URunAwayItemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URunAwayItemComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    // --- チューニング項目 ---
    UPROPERTY(EditAnywhere, Category = "Orilen|Flee")
    int32 RayCount = 16;

    UPROPERTY(EditAnywhere, Category = "Orilen|Flee")
    float RayLength = 150.f;

    UPROPERTY(EditAnywhere, Category = "Orilen|Flee")
    float DetectRadius = 400.f;

    UPROPERTY(EditAnywhere, Category = "Orilen|Flee")
    float LeaveRadius = 600.f;

    UPROPERTY(EditAnywhere, Category = "Orilen|Flee")
    float FleeSpeed = 300.f;

    UPROPERTY(EditAnywhere, Category = "Orilen|Flee", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RandomStrength = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Orilen|Flee")
    float RandomInterval = 0.5f;

    // --- 内部状態 ---
    bool bFleeing = false;
    FVector RandomOffset = FVector::ZeroVector;
    float RandomTimer = 0.f;

    TWeakObjectPtr<AActor> PlayerRef;

    // --- 処理 ---
    FVector ComputeFleeDirection() const;
    FVector ComputeWallAvoidance() const;
    void UpdateRandomOffset(float DeltaTime);

    // フィボナッチ球面サンプリングで均等分散したレイ方向を生成
    TArray<FVector> GetSphereRayDirections() const;
};