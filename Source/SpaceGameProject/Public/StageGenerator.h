// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageGenerator.generated.h"

class USplineComponent;
class AItemParent;
class UStaticMesh;

UCLASS()
class SPACEGAMEPROJECT_API AStageGenerator : public AActor
{
	GENERATED_BODY()

public:
	AStageGenerator();

	// エディタでプレイヤーの経路を編集するためのスプライン
	UPROPERTY(VisibleAnywhere, Category = "StageGeneration")
	USplineComponent* PathSpline = nullptr;

	UPROPERTY(EditAnywhere, Category = "StageGeneration")
	TSubclassOf<AItemParent> ItemClass;

	// 生成するアイテム(ItemParent)の総数
	UPROPERTY(EditAnywhere, Category = "StageGeneration", meta = (ClampMin = "0"))
	int32 TargetItemCount = 20;

	// デブリ(装飾メッシュ)を列挙するContent Browser上のフォルダ
	UPROPERTY(EditAnywhere, Category = "StageGeneration", meta = (ContentDir))
	FDirectoryPath DebrisMeshFolder;

	// 生成するデブリの総数
	UPROPERTY(EditAnywhere, Category = "StageGeneration", meta = (ClampMin = "0"))
	int32 TargetDebrisCount = 15;

	// 生成オブジェクトのスケール範囲(X=Min, Y=Max)
	UPROPERTY(EditAnywhere, Category = "StageGeneration", meta = (ClampMin = "0.01"))
	FVector2D ScaleRange = FVector2D(1.f, 1.f);

	UPROPERTY(EditAnywhere, Category = "StageGeneration")
	int32 RandomSeed = 0;

	// カーブのサンプリング間隔
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Grid")
	float SplineSampleSpacing = 100.f;

	// グリッドセルのサイズ
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Grid")
	float CellSize = 200.f;

	// カーブ隣接判定の半径(セル単位)
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Grid")
	float CurveAdjacentRadius = 2.f;

	// カーブ隣接セルから候補エリアへ円形に拡張する半径(セル単位)
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Grid")
	float ExpansionRadius = 3.f;

	// セル中心からのランダムオフセット最大距離
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Grid")
	float MaxOffsetRange = 80.f;

	// 核(クラスタの中心)の数
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Clustering")
	int32 CoreCount = 3;

	// 核同士の最小間隔(セル単位)
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Clustering")
	float CoreMinSpacing = 4.f;

	// 核選出のリトライ上限(超えたらその核は諦める)
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Clustering")
	int32 CoreSelectionRetryLimit = 50;

	// 核への引力の強さ(0〜1、距離に応じた線形補間の係数)
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Clustering", meta = (ClampMin = "0", ClampMax = "1"))
	float AttractionStrength = 1.f;

	// 最終配置時にこの距離未満は重複として棄却
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Clustering")
	float MinObjectSpacing = 100.f;

	// Itemが絡む場合の最小間隔(MinObjectSpacingより優先。均等に間引くために使う)
	UPROPERTY(EditAnywhere, Category = "StageGeneration|Clustering")
	float ItemMinSpacing = 300.f;

	// 再生成時にクリアする対象を追跡する
	UPROPERTY()
	TArray<AActor*> GeneratedActors;

	UFUNCTION(CallInEditor, Category = "StageGeneration")
	void Generate();

	UFUNCTION(CallInEditor, Category = "StageGeneration")
	void ClearGenerated();

private:
	enum class EPlacedObjectType : uint8
	{
		Item,
		Debris
	};

	struct FPlacedObject
	{
		FVector Location = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;
		EPlacedObjectType Type = EPlacedObjectType::Item;
		UStaticMesh* DebrisMesh = nullptr;
		float Scale = 1.f;
	};

	TArray<FVector> SampleSplinePoints() const;
	FVector CellToWorld(const FIntVector& Cell, const FVector& GridOrigin) const;

	// 核への引力/重み付けが届く範囲
	float GetAttractionRadius() const;

#if WITH_EDITOR
	// DebrisMeshFolder配下のUStaticMeshを全列挙する
	TArray<UStaticMesh*> LoadDebrisMeshes() const;
#endif

	// step1+2: カーブ隣接セル抽出 → 円形拡張した候補エリアを返す
	TArray<FIntVector> BuildCandidateCells(const TArray<FVector>& SplineSamples, FVector& OutGridOrigin) const;

	// step4: 最小間隔を満たす核をリトライ方式で選出
	TArray<FIntVector> SelectCores(const TArray<FIntVector>& CandidateCells, FRandomStream& RandomStream) const;

	// 核への近さで重み付けしたセルから、重複なしでCount個を抽選する
	TArray<FIntVector> WeightedSampleCells(TArray<TPair<FIntVector, float>> WeightedCells, int32 Count, FRandomStream& RandomStream) const;

	// step2+3: 核周辺のセルを重み付き抽選し、オフセット配置+角度ランダム化
	TArray<FPlacedObject> PlaceObjectsInCells(const TArray<FIntVector>& CandidateCells, const FVector& GridOrigin, const TArray<FIntVector>& Cores, const TArray<UStaticMesh*>& DebrisMeshes, FRandomStream& RandomStream) const;

	// step5: 最寄りの核へ距離依存の線形補間で引き寄せる
	void ApplyClustering(TArray<FPlacedObject>& Objects, const TArray<FIntVector>& Cores, const FVector& GridOrigin) const;

	// step6: ブルートフォースで最小距離未満を棄却
	TArray<FPlacedObject> ResolveOverlaps(const TArray<FPlacedObject>& Objects) const;
};
