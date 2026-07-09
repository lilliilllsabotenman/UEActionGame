//// BranchGrowthComponent.cpp
//#include "BranchGrowthComponent.h"
//#include "Engine/StaticMeshActor.h"
//
//void UBranchGrowthComponent::GrowBranch()
//{
//    ClearBranch();
//
//    FRandomStream Rand(Seed == 0 ? FMath::Rand() : Seed);
//
//    Segments.Empty();
//
//    // 起点はオーナーアクターのTransform
//    AActor* Owner = GetOwner();
//    FVector  CurrentPos = Owner->GetActorLocation();
//    FRotator CurrentRot = Owner->GetActorRotation();
//
//    for (int32 i = 0; i < MaxDepth; i++)
//    {
//        FBranchSegment Seg;
//        Seg.WorldPosition = CurrentPos;
//        Seg.WorldRotation = CurrentRot;
//        Seg.Depth = i;
//        Segments.Add(Seg);
//
//        SpawnSegmentMesh(Seg, Rand);
//
//        // 次のセグメントへ進む
//        float Length = Rand.FRandRange(SegmentLengthMin, SegmentLengthMax);
//
//        // 現在の向きに角度ランダムを加算
//        FRotator DeltaRot(
//            Rand.FRandRange(PitchMin, PitchMax),
//            Rand.FRandRange(YawMin, YawMax),
//            Rand.FRandRange(RollMin, RollMax)
//        );
//
//        CurrentRot = (CurrentRot.Quaternion() * DeltaRot.Quaternion()).Rotator();
//        CurrentPos += CurrentRot.Vector() * Length;
//    }
//}
//
//void UBranchGrowthComponent::SpawnSegmentMesh(const FBranchSegment& Seg, FRandomStream& Rand)
//{
//    if (SegmentMeshes.IsEmpty()) return;
//
//    UWorld* World = GetWorld();
//    if (!World) return;
//
//    UStaticMesh* Mesh = SegmentMeshes[Rand.RandRange(0, SegmentMeshes.Num() - 1)];
//
//    FActorSpawnParameters Params;
//    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//    AStaticMeshActor* SMA = World->SpawnActor<AStaticMeshActor>(
//        AStaticMeshActor::StaticClass(),
//        Seg.WorldPosition,
//        Seg.WorldRotation,
//        Params
//    );
//
//    if (SMA)
//    {
//        SMA->GetStaticMeshComponent()->SetStaticMesh(Mesh);
//        SMA->SetMobility(EComponentMobility::Static);
//        SpawnedActors.Add(SMA);
//    }
//}
//
//void UBranchGrowthComponent::ClearBranch()
//{
//    for (AActor* A : SpawnedActors)
//    {
//        if (IsValid(A)) A->Destroy();
//    }
//    SpawnedActors.Empty();
//    Segments.Empty();
//}