#include "RopeSolver.h"

FVector RopeSolver::SolveForce(
	const FVector& ControlledPosition,
	const FVector& ControlledVelocity,
	const FVector& HookPosition,
	const FRopeSettings& Settings
)
{
	FVector ToHook = HookPosition - ControlledPosition;

	float Dist = ToHook.Size();

	if (Dist <= KINDA_SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	FVector Dir = ToHook.GetSafeNormal();

	FVector TotalForce = FVector::ZeroVector;

	float Stretch = Dist - Settings.RopeLength;

	if (Stretch > 0.f)
	{
		float VelAlong = FVector::DotProduct(ControlledVelocity, Dir);

		float ForceMag = Stretch * Settings.SpringK - VelAlong * Settings.Damping;

		TotalForce = Dir * ForceMag;
	}

	return TotalForce;
}

FQuat RopeSolver::SolveRotation(
	const FVector& PullingForce,
	const FVector& Forward
)
{
	if (PullingForce.IsNearlyZero())
	{
		return FQuat::Identity;
	}

	FVector TargetUp = -PullingForce.GetSafeNormal();

	// Forward が TargetUp とほぼ平行だと MakeFromZX が縮退するのでフォールバック軸に切り替える
	FVector Hint = Forward;
	if (FVector::CrossProduct(TargetUp, Hint).IsNearlyZero())
	{
		Hint = FVector::CrossProduct(TargetUp, FVector::UpVector).IsNearlyZero()
			? FVector::ForwardVector
			: FVector::UpVector;
	}

	FMatrix RotMatrix = FRotationMatrix::MakeFromZX(TargetUp, Hint);
	return RotMatrix.ToQuat();
}
