// HookGuideFinder.h
#pragma once

#include "CoreMinimal.h"
#include "RopeSetting.h"

class SPACEGAMEPROJECT_API HookGuideFinder
{
public:
	// カメラ正面のレイ＋指定角度に扇状に散らしたレイをMaxRopeLengthまで飛ばす。
	// AHookGuideに当たったレイがあればそのActor位置を、無ければ正面レイのヒット点を返す。
	// 正面レイが何にも当たらなければfalse。
	static bool FindHookTarget(
		UWorld* World,
		const FVector& RayOrigin,
		const FVector& ForwardDirection,
		AActor* IgnoreActor,
		const FRopeSettings& Settings,
		FVector& OutTargetPosition);

private:
	// Forwardからコーン角ConeAngleDegrees・方位角AzimuthDegreesだけ傾けた方向を求める
	static FVector ComputeConeRayDirection(const FVector& Forward, float ConeAngleDegrees, float AzimuthDegrees);
};
