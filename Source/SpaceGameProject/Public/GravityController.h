#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GravityController.generated.h"

UCLASS()
class AGravityController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void UpdateRotation(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	UFUNCTION(BlueprintPure)
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

	// 重力方向の変化をこの速さ(度/秒)で追従させる。カメラ基準の切り替えを滑らかにする。
	UPROPERTY(EditDefaultsOnly, Category = "Gravity")
	float GravityEaseSpeed = 180.f;

private:

	// UpdateRotationが実際に基準として使う、滑らかに追従した重力方向。
	FVector SmoothedGravityDirection = FVector::DownVector;
};