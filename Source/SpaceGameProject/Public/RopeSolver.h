#pragma once

#include "CoreMinimal.h"
#include "RopeSetting.h"

// Pure spring-damper force calculation for the rope. No engine/actor dependencies.
class SPACEGAMEPROJECT_API RopeSolver
{
public:

	static FVector SolveForce(
		const FVector& ControlledPosition,
		const FVector& ControlledVelocity,
		const FVector& HookPosition,
		const FRopeSettings& Settings
	);

	// Leans "up" away from the pulling force, expressed as a delta rotation from CurrentUp
	// to the target up so it can be composed additively (e.g. via IRotationEventHub).
	// Identity when PullingForce is (near) zero.
	static FQuat SolveRotation(
		const FVector& PullingForce,
		const FVector& CurrentUp
	);
};