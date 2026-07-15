// RopeSettings.h

#pragma once

#include "CoreMinimal.h"
#include "RopeSetting.generated.h"

USTRUCT(BlueprintType)
struct SPACEGAMEPROJECT_API FRopeSettings
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float SpringK = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float Damping = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float ConstraintStrength = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float ReelSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float ReelRate = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float RopeLength = 1000.f;

	// Max trace distance when acquiring a hook target. RopeLength itself gets overwritten with the actual hit distance once hooked.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float MaxRopeLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float Gravity = -980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float SpringStrength = 50.f;

	// 重心から見た、ロープが引っ張る位置のローカルオフセット。ここがゼロだと回転が一切発生しない。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	FVector LocalAttachOffset = FVector(0.f, -20.f, 60.f);

	// 回転のしやすさ(慣性)。大きいほど同じ力でもゆっくり回る。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple", meta = (ClampMin = "0.001"))
	float RotationalInertia = 400000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple", meta = (ClampMin = "0.0"))
	float RotDamping = 8.f;

	// 射出アニメーションの見た目上の速度(cm/s)。当たり判定には影響しない。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple", meta = (ClampMin = "0.0"))
	float ShootSpeed = 4000.f;
};