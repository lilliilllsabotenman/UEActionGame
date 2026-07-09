//// BranchGrowthComponent.h
//#pragma once
//#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "BranchGrowthComponent.generated.h"
//
//USTRUCT(BlueprintType)
//struct FBranchSegment
//{
//    GENERATED_BODY()
//
//    FVector  WorldPosition;
//    FRotator WorldRotation;
//    int32    Depth;
//};
//
//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
//class SPACEGAME_API UBranchGrowthComponent : public UActorComponent
//{
//    GENERATED_BODY()
//
//public:
//    // ---- 長さ・深さ ----
//    UPROPERTY(EditAnywhere, Category = "Branch|Growth")
//    int32 MaxDepth = 10;                  // 積み上げるセグメント数
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Growth")
//    float SegmentLengthMin = 300.f;
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Growth")
//    float SegmentLengthMax = 600.f;
//
//    // ---- 角度ランダム幅 ----
//    UPROPERTY(EditAnywhere, Category = "Branch|Angle", meta = (ClampMin = "-180", ClampMax = "180"))
//    float YawMin = -40.f;
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Angle", meta = (ClampMin = "-180", ClampMax = "180"))
//    float YawMax = 40.f;
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Angle", meta = (ClampMin = "-90", ClampMax = "90"))
//    float PitchMin = -30.f;
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Angle", meta = (ClampMin = "-90", ClampMax = "90"))
//    float PitchMax = 30.f;
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Angle", meta = (ClampMin = "-180", ClampMax = "180"))
//    float RollMin = -15.f;
//
//    UPROPERTY(EditAnywhere, Category = "Branch|Angle", meta = (ClampMin = "-180", ClampMax = "180"))
//    float RollMax = 15.f;
//
//    // ---- メッシュ ----
//    UPROPERTY(EditAnywhere, Category = "Branch|Mesh")
//    TArray<TObjectPtr<UStaticMesh>> SegmentMeshes;
//
//    // ---- シード ----
//    UPROPERTY(EditAnywhere, Category = "Branch|Seed")
//    int32 Seed = 0;   // 0 = 毎回ランダム
//
//    // ---- Editor ボタン ----
//    UFUNCTION(CallInEditor, Category = "Branch")
//    void GrowBranch();
//
//    UFUNCTION(CallInEditor, Category = "Branch")
//    void ClearBranch();
//
//    // 生成されたセグメント情報（外部から参照可能）
//    UPROPERTY(VisibleAnywhere, Category = "Branch|Debug")
//    TArray<FBranchSegment> Segments;
//
//private:
//    UPROPERTY()
//    TArray<TObjectPtr<AActor>> SpawnedActors;
//
//    void SpawnSegmentMesh(const FBranchSegment& Seg, FRandomStream& Rand);
//};