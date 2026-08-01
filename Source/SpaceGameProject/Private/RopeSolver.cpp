#include "RopeSolver.h"
#include "Engine/Engine.h"


FVector RopeSolver::SolveForce(
	const FVector& ControlledPosition,
	const FVector& ControlledVelocity,
	const FVector& HookPosition,
	const FRopeSettings& Settings)
{
	FVector ToHook = HookPosition - ControlledPosition;
	float Dist = ToHook.Size();

	if (Dist <= KINDA_SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	FVector Dir = ToHook.GetSafeNormal();
	float Stretch = Dist - Settings.RopeLength;

	if (Stretch <= 0.f)
	{
		return FVector::ZeroVector;
	}

	float VelAlong = FVector::DotProduct(ControlledVelocity, Dir);
	float ForceMag = Stretch * Settings.SpringK - VelAlong * Settings.Damping;

	return Dir * ForceMag;
}


FVector RopeSolver::SolveDirection(
		FVector CurrentUpDirection,
		FVector TargetDirection,
		FVector SolveSpeed)
{
	float MaximSpped = 500000.f;

	// Float
	// FVector
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, CurrentUpDirection.ToString());

	float ClampedValue = FMath::Clamp(SolveSpeed.Size() / MaximSpped, 0.0f, 1.0f);
	return FMath::Lerp(CurrentUpDirection, TargetDirection, ClampedValue);
}
