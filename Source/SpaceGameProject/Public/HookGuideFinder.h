// HookGuideFinder.h
#pragma once

#include "CoreMinimal.h"
#include "RopeSetting.h"

class SPACEGAMEPROJECT_API HookGuideFinder
{
public:
	// カメラ正面のレイをMaxRopeLengthまで飛ばす。AHookGuideに当たればそのActor位置を採用する。
	// 外れた場合は、正面からHookAssistSpreadAngle以内・視線が通るHookGuideのうち
	// 正面に一番近い角度のものにスナップする。それも無ければ正面レイのヒット点を返す。
	// 正面レイが何にも当たらなければfalse。
	static bool FindHookTarget(
		UWorld* World,
		const FVector& RayOrigin,
		const FVector& ForwardDirection,
		AActor* IgnoreActor,
		const FRopeSettings& Settings,
		FVector& OutTargetPosition);
};
