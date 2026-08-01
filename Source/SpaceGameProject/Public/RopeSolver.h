// RopeSolver.h
#pragma once

#include "CoreMinimal.h"
#include "RopeSetting.h"

class SPACEGAMEPROJECT_API RopeSolver
{
public:
	static FVector SolveForce(
		const FVector& ControlledPosition,
		const FVector& ControlledVelocity,
		const FVector& HookPosition,
		const FRopeSettings& Settings);

	// 遠心力から目標バンク方向を求め、CurrentUp→TargetUpへの変化量(世界空間delta)を
	// FQuatとして直接返す。stateless。AddQuatにそのまま渡せる。
	static FVector SolveDirection(
		FVector CurrentUpDirection,
		FVector TargetDirection,
		FVector SolveSpeed);
};
