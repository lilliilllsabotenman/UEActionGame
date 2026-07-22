// Fill out your copyright notice in the Description page of Project Settings.
// ScanComponent.h

#pragma once

#include "CharacterComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectTracker.h"
#include "ScanComponent.generated.h"

class UInputAction;
class UEnhancedInputComponent;
class UCameraComponent;
class UUserWidget;
class UObjectTracker;
class UPrimitiveComponent;
class APlayerController;

// 固定本数のサンプルスロット1個分の状態。スロットは毎フレーム別の位置を再抽選してトレースし直すため、
// ヒット位置そのものは保持せず「登録済みかどうか」だけ管理する。
USTRUCT()
struct FScanMarkerState
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* MarkerKey = nullptr;

	bool bIsRegistered = false;
};

// デスストのスキャンっぽいもののごり押し実装。
// 画面中心から近いほど密になるようサンプル点を毎フレーム再抽選し、カメラからレイトレースする。
// 常時ONのミニレーダー的な挙動で、ScanActionでON/OFFをトグルする。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UScanComponent : public UActorComponent, public ICharacterComponent
{
	GENERATED_BODY()

public:
	UScanComponent();

	virtual void BindInput(UEnhancedInputComponent* EnhancedInput) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//=======Input============

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Input")
	UInputAction* ScanAction = nullptr;

//=======Marker============

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
	TSubclassOf<UUserWidget> MarkerWidgetClass;

	// 距離に応じたマーカーの拡大縮小設定(ItemTrackerComponentと同じ仕組み)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan")
	FDistanceScaleSettings ScaleSettings;

//=======Trace============

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan|Trace", meta = (ClampMin = "0.0"))
	float ScanRange = 5000.0f;

	// このクラス(派生含む)にヒットしてもスキャン対象から除外する
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan|Trace")
	TArray<TSubclassOf<AActor>> ExcludedActorClasses;

//=======Sampling============

	// 常時稼働させるサンプルスロット数(多いほど密だが毎フレームのトレース数が増える)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan|Sampling", meta = (ClampMin = "1"))
	int32 SampleCount = 300;

	// 画面中心からのサンプル半径の偏りを決めるべき指数。1.0で一様分布、大きいほど中心(近く)に偏る
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scan|Sampling", meta = (ClampMin = "0.1"))
	float DensityExponent = 3.0f;

	// 開始/停止をトグルする
	void StartScan();

private:

	UPROPERTY()
	UObjectTracker* Tracker = nullptr;

	UPROPERTY()
	APlayerController* CachedPC = nullptr;

	UPROPERTY()
	TArray<FScanMarkerState> Slots;

	bool bIsScanning = false;

	void UpdateSampleSlots();
	bool TraceFromScreenPoint(const FVector2D& ScreenPos, FHitResult& OutHit) const;
	void DeactivateSlot(FScanMarkerState& Slot);
	bool IsExcluded(const AActor* Actor) const;
};
