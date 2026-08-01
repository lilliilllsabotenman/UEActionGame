// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntiGravityFloor.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UBoxComponent;

// 無重力の切り替えに使う床。IsAntiGravityで役割が固定される。
// true: 無重力に入るだけの床(すでに無重力なら何もしない)
// false: 無重力から復帰するだけの床(無重力状態のときだけ効く)
// 判定自体はChangeGravityComponent側で行い、このクラスはメッシュと役割・見た目を持つだけのマーカー。
UCLASS()
class SPACEGAMEPROJECT_API AAntiGravityFloor : public AActor
{
	GENERATED_BODY()

public:
	AAntiGravityFloor();

	UPROPERTY(VisibleAnywhere, Category = "AntiGravityFloor")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 侵入検知用のエリア。サイズは見た目のMeshと独立して調整可能
	UPROPERTY(VisibleAnywhere, Category = "AntiGravityFloor")
	TObjectPtr<UBoxComponent> TriggerVolume;

	// true: 無重力化専用、false: 重力復帰専用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiGravityFloor")
	bool IsAntiGravity = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiGravityFloor")
	TObjectPtr<UMaterialInterface> AntiGravityMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AntiGravityFloor")
	TObjectPtr<UMaterialInterface> GravityMaterial;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	void UpdateMaterial();

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
