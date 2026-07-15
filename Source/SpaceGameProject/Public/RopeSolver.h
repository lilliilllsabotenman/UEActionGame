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

	static FQuat StepRotation(
		const FQuat& CurrentRotation,
		FVector& AngularVelocity,
		const FVector& Force,
		const FRopeSettings& Settings,
		float DeltaTime);
};