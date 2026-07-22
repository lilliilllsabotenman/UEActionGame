// Fill out your copyright notice in the Description page of Project Settings.

#include "StageGenerator.h"
#include "Components/SplineComponent.h"
#include "ItemParent.h"
#include "Math/Box.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "SpaceGameProject.h"

#if WITH_EDITOR
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetData.h"
#endif

AStageGenerator::AStageGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	RootComponent = PathSpline;
}

TArray<FVector> AStageGenerator::SampleSplinePoints() const
{
	TArray<FVector> Samples;

	const float SplineLength = PathSpline->GetSplineLength();
	if (SplineLength <= 0.f || SplineSampleSpacing <= 0.f)
	{
		return Samples;
	}

	for (float Distance = 0.f; Distance <= SplineLength; Distance += SplineSampleSpacing)
	{
		Samples.Add(PathSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World));
	}
	Samples.Add(PathSpline->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::World));

	return Samples;
}

FVector AStageGenerator::CellToWorld(const FIntVector& Cell, const FVector& GridOrigin) const
{
	return GridOrigin + FVector(Cell) * CellSize + FVector(CellSize * 0.5f);
}

float AStageGenerator::GetAttractionRadius() const
{
	return (CurveAdjacentRadius + ExpansionRadius) * CellSize;
}

#if WITH_EDITOR
TArray<UStaticMesh*> AStageGenerator::LoadDebrisMeshes() const
{
	TArray<UStaticMesh*> Meshes;

	if (DebrisMeshFolder.Path.IsEmpty())
	{
		return Meshes;
	}

	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	FARFilter Filter;
	Filter.PackagePaths.Add(FName(*DebrisMeshFolder.Path));
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UStaticMesh::StaticClass()->GetClassPathName());

	TArray<FAssetData> AssetDatas;
	AssetRegistry.GetAssets(Filter, AssetDatas);

	for (const FAssetData& AssetData : AssetDatas)
	{
		if (UStaticMesh* Mesh = Cast<UStaticMesh>(AssetData.GetAsset()))
		{
			Meshes.Add(Mesh);
		}
	}

	return Meshes;
}
#endif

TArray<FIntVector> AStageGenerator::BuildCandidateCells(const TArray<FVector>& SplineSamples, FVector& OutGridOrigin) const
{
	TArray<FIntVector> CandidateCells;
	if (SplineSamples.Num() == 0 || CellSize <= 0.f)
	{
		return CandidateCells;
	}

	// カーブサンプル点全体を包むバウンズを、拡張分のマージンを取って算出
	FBox Bounds(SplineSamples);
	const float MarginWorld = (CurveAdjacentRadius + ExpansionRadius + 1.f) * CellSize;
	Bounds = Bounds.ExpandBy(MarginWorld);

	OutGridOrigin = Bounds.Min;

	const FIntVector GridMin(0, 0, 0);
	const FIntVector GridMax(
		FMath::CeilToInt(Bounds.GetSize().X / CellSize),
		FMath::CeilToInt(Bounds.GetSize().Y / CellSize),
		FMath::CeilToInt(Bounds.GetSize().Z / CellSize));

	// step1: カーブ隣接セルを総当たりで抽出
	TSet<FIntVector> CurveAdjacentCells;
	const float CurveRadiusWorldSq = FMath::Square(CurveAdjacentRadius * CellSize);
	for (int32 X = GridMin.X; X <= GridMax.X; ++X)
	{
		for (int32 Y = GridMin.Y; Y <= GridMax.Y; ++Y)
		{
			for (int32 Z = GridMin.Z; Z <= GridMax.Z; ++Z)
			{
				const FIntVector Cell(X, Y, Z);
				const FVector CellCenter = CellToWorld(Cell, OutGridOrigin);

				float MinDistSq = TNumericLimits<float>::Max();
				for (const FVector& Sample : SplineSamples)
				{
					MinDistSq = FMath::Min(MinDistSq, FVector::DistSquared(CellCenter, Sample));
				}

				if (MinDistSq <= CurveRadiusWorldSq)
				{
					CurveAdjacentCells.Add(Cell);
				}
			}
		}
	}

	// step2: カーブ隣接セルを起点に、円形に候補エリアを拡張(ディレーション)
	TSet<FIntVector> ExpandedCells = CurveAdjacentCells;
	if (ExpansionRadius > 0.f)
	{
		const int32 IntRadius = FMath::CeilToInt(ExpansionRadius);
		const float ExpansionRadiusSq = FMath::Square(ExpansionRadius);

		for (const FIntVector& Base : CurveAdjacentCells)
		{
			for (int32 DX = -IntRadius; DX <= IntRadius; ++DX)
			{
				for (int32 DY = -IntRadius; DY <= IntRadius; ++DY)
				{
					for (int32 DZ = -IntRadius; DZ <= IntRadius; ++DZ)
					{
						if (FVector(DX, DY, DZ).SizeSquared() <= ExpansionRadiusSq)
						{
							ExpandedCells.Add(Base + FIntVector(DX, DY, DZ));
						}
					}
				}
			}
		}
	}

	CandidateCells = ExpandedCells.Array();
	return CandidateCells;
}

TArray<FIntVector> AStageGenerator::SelectCores(const TArray<FIntVector>& CandidateCells, FRandomStream& RandomStream) const
{
	TArray<FIntVector> Cores;
	if (CandidateCells.Num() == 0 || CoreCount <= 0)
	{
		return Cores;
	}

	for (int32 CoreIndex = 0; CoreIndex < CoreCount; ++CoreIndex)
	{
		for (int32 Attempt = 0; Attempt < CoreSelectionRetryLimit; ++Attempt)
		{
			const FIntVector Candidate = CandidateCells[RandomStream.RandRange(0, CandidateCells.Num() - 1)];

			bool bFarEnough = true;
			for (const FIntVector& Existing : Cores)
			{
				if (FVector(Candidate - Existing).Size() < CoreMinSpacing)
				{
					bFarEnough = false;
					break;
				}
			}

			if (bFarEnough)
			{
				Cores.Add(Candidate);
				break;
			}
			// リトライ上限に達した場合はこの核を諦める(核数がCoreCount未満になることを許容)
		}
	}

	return Cores;
}

TArray<FIntVector> AStageGenerator::WeightedSampleCells(TArray<TPair<FIntVector, float>> WeightedCells, int32 Count, FRandomStream& RandomStream) const
{
	TArray<FIntVector> Selected;
	Count = FMath::Min(Count, WeightedCells.Num());

	for (int32 Index = 0; Index < Count; ++Index)
	{
		float TotalWeight = 0.f;
		for (const TPair<FIntVector, float>& Pair : WeightedCells)
		{
			TotalWeight += Pair.Value;
		}

		if (TotalWeight <= 0.f)
		{
			break;
		}

		const float Roll = RandomStream.FRandRange(0.f, TotalWeight);
		float Cumulative = 0.f;
		int32 ChosenIndex = WeightedCells.Num() - 1;
		for (int32 CellIndex = 0; CellIndex < WeightedCells.Num(); ++CellIndex)
		{
			Cumulative += WeightedCells[CellIndex].Value;
			if (Roll <= Cumulative)
			{
				ChosenIndex = CellIndex;
				break;
			}
		}

		Selected.Add(WeightedCells[ChosenIndex].Key);
		WeightedCells.RemoveAtSwap(ChosenIndex);
	}

	return Selected;
}

TArray<AStageGenerator::FPlacedObject> AStageGenerator::PlaceObjectsInCells(const TArray<FIntVector>& CandidateCells, const FVector& GridOrigin, const TArray<FIntVector>& Cores, const TArray<UStaticMesh*>& DebrisMeshes, FRandomStream& RandomStream) const
{
	TArray<FPlacedObject> Objects;

	TArray<FVector> CoreLocations;
	CoreLocations.Reserve(Cores.Num());
	for (const FIntVector& Core : Cores)
	{
		CoreLocations.Add(CellToWorld(Core, GridOrigin));
	}

	const float AttractionRadius = GetAttractionRadius();

	// 核への近さでセルに重みを付ける(核から離れたセルは重み0=対象外にして塊を作る)
	TArray<TPair<FIntVector, float>> WeightedCells;
	WeightedCells.Reserve(CandidateCells.Num());
	if (CoreLocations.Num() > 0 && AttractionRadius > 0.f)
	{
		for (const FIntVector& Cell : CandidateCells)
		{
			const FVector CellCenter = CellToWorld(Cell, GridOrigin);

			float NearestDist = TNumericLimits<float>::Max();
			for (const FVector& CoreLocation : CoreLocations)
			{
				NearestDist = FMath::Min(NearestDist, FVector::Dist(CellCenter, CoreLocation));
			}

			if (NearestDist >= AttractionRadius)
			{
				continue;
			}

			WeightedCells.Add(TPair<FIntVector, float>(Cell, 1.f - (NearestDist / AttractionRadius)));
		}
	}

	auto SpawnFromSelection = [this, &Objects, &GridOrigin, &RandomStream, &DebrisMeshes](const TArray<FIntVector>& SelectedCells, EPlacedObjectType Type)
	{
		for (const FIntVector& Cell : SelectedCells)
		{
			const FVector CellCenter = CellToWorld(Cell, GridOrigin);
			const FVector Offset = RandomStream.VRand() * (RandomStream.FRand() * MaxOffsetRange);
			const FRotator Rotation(RandomStream.FRandRange(0.f, 360.f), RandomStream.FRandRange(0.f, 360.f), RandomStream.FRandRange(0.f, 360.f));

			FPlacedObject Obj;
			Obj.Location = CellCenter + Offset;
			Obj.Rotation = Rotation;
			Obj.Type = Type;
			Obj.Scale = RandomStream.FRandRange(ScaleRange.X, ScaleRange.Y);
			if (Type == EPlacedObjectType::Debris && DebrisMeshes.Num() > 0)
			{
				Obj.DebrisMesh = DebrisMeshes[RandomStream.RandRange(0, DebrisMeshes.Num() - 1)];
			}
			Objects.Add(Obj);
		}
	};

	if (ItemClass && TargetItemCount > 0)
	{
		SpawnFromSelection(WeightedSampleCells(WeightedCells, TargetItemCount, RandomStream), EPlacedObjectType::Item);
	}

	if (DebrisMeshes.Num() > 0 && TargetDebrisCount > 0)
	{
		SpawnFromSelection(WeightedSampleCells(WeightedCells, TargetDebrisCount, RandomStream), EPlacedObjectType::Debris);
	}

	return Objects;
}

void AStageGenerator::ApplyClustering(TArray<FPlacedObject>& Objects, const TArray<FIntVector>& Cores, const FVector& GridOrigin) const
{
	if (Cores.Num() == 0 || AttractionStrength <= 0.f)
	{
		return;
	}

	TArray<FVector> CoreLocations;
	CoreLocations.Reserve(Cores.Num());
	for (const FIntVector& Core : Cores)
	{
		CoreLocations.Add(CellToWorld(Core, GridOrigin));
	}

	const float AttractionRadius = GetAttractionRadius();
	if (AttractionRadius <= 0.f)
	{
		return;
	}

	for (FPlacedObject& Obj : Objects)
	{
		float NearestDist = TNumericLimits<float>::Max();
		FVector NearestCore = FVector::ZeroVector;
		for (const FVector& CoreLocation : CoreLocations)
		{
			const float Dist = FVector::Dist(Obj.Location, CoreLocation);
			if (Dist < NearestDist)
			{
				NearestDist = Dist;
				NearestCore = CoreLocation;
			}
		}

		if (NearestDist >= AttractionRadius)
		{
			continue;
		}

		// 核に近いほど強く引っ張る(距離に応じた線形補間)
		const float Falloff = 1.f - (NearestDist / AttractionRadius);
		const float Alpha = FMath::Clamp(AttractionStrength * Falloff, 0.f, 1.f);
		Obj.Location = FMath::Lerp(Obj.Location, NearestCore, Alpha);
	}
}

TArray<AStageGenerator::FPlacedObject> AStageGenerator::ResolveOverlaps(const TArray<FPlacedObject>& Objects) const
{
	TArray<FPlacedObject> Accepted;
	Accepted.Reserve(Objects.Num());

	for (const FPlacedObject& Obj : Objects)
	{
		bool bFarEnough = true;
		for (const FPlacedObject& Other : Accepted)
		{
			// Itemが絡む場合はItemMinSpacingを優先して間引く(均等に間隔を取るため)
			const bool bInvolvesItem = (Obj.Type == EPlacedObjectType::Item) || (Other.Type == EPlacedObjectType::Item);
			const float RequiredSpacing = bInvolvesItem ? ItemMinSpacing : MinObjectSpacing;

			if (FVector::DistSquared(Obj.Location, Other.Location) < FMath::Square(RequiredSpacing))
			{
				bFarEnough = false;
				break;
			}
		}

		if (bFarEnough)
		{
			Accepted.Add(Obj);
		}
	}

	return Accepted;
}

void AStageGenerator::Generate()
{
	ClearGenerated();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const TArray<FVector> SplineSamples = SampleSplinePoints();
	if (SplineSamples.Num() == 0)
	{
		return;
	}

	FVector GridOrigin = FVector::ZeroVector;
	const TArray<FIntVector> CandidateCells = BuildCandidateCells(SplineSamples, GridOrigin);

	FRandomStream RandomStream(RandomSeed);

#if WITH_EDITOR
	const TArray<UStaticMesh*> DebrisMeshes = LoadDebrisMeshes();
#else
	const TArray<UStaticMesh*> DebrisMeshes;
#endif

	const TArray<FIntVector> Cores = SelectCores(CandidateCells, RandomStream);
	TArray<FPlacedObject> Objects = PlaceObjectsInCells(CandidateCells, GridOrigin, Cores, DebrisMeshes, RandomStream);
	ApplyClustering(Objects, Cores, GridOrigin);
	const TArray<FPlacedObject> FinalObjects = ResolveOverlaps(Objects);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	for (const FPlacedObject& Obj : FinalObjects)
	{
		switch (Obj.Type)
		{
		case EPlacedObjectType::Debris:
			if (Obj.DebrisMesh)
			{
				if (AStaticMeshActor* Debris = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Obj.Location, Obj.Rotation, SpawnParams))
				{
					if (UStaticMeshComponent* MeshComp = Debris->GetStaticMeshComponent())
					{
						MeshComp->SetStaticMesh(Obj.DebrisMesh);
					}
					Debris->SetActorScale3D(FVector(Obj.Scale));
					GeneratedActors.Add(Debris);
				}
			}
			break;

		case EPlacedObjectType::Item:
		default:
			if (AActor* Item = World->SpawnActor<AItemParent>(ItemClass, Obj.Location, Obj.Rotation, SpawnParams))
			{
				Item->SetActorScale3D(FVector(Obj.Scale));
				GeneratedActors.Add(Item);
			}
			else
			{
				UE_LOG(LogSpaceGameProject, Warning, TEXT("StageGenerator: ItemClass(%s)のスポーンに失敗 at %s"), *GetNameSafe(ItemClass), *Obj.Location.ToString());
			}
			break;
		}
	}

	UE_LOG(LogSpaceGameProject, Log, TEXT("StageGenerator: Candidates=%d Cores=%d PlacedBeforeOverlap=%d Final=%d ItemClass=%s TargetItemCount=%d"),
		CandidateCells.Num(), Cores.Num(), Objects.Num(), FinalObjects.Num(), *GetNameSafe(ItemClass), TargetItemCount);
}

void AStageGenerator::ClearGenerated()
{
	for (AActor* Actor : GeneratedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	GeneratedActors.Empty();
}
