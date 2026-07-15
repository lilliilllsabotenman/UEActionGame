#include "RopeSolver.h"

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

FQuat RopeSolver::StepRotation(
	const FQuat& CurrentRotation,
	FVector& AngularVelocity,
	const FVector& Force,
	const FRopeSettings& Settings,
	float DeltaTime)
{
	FVector AttachOffsetWorld = CurrentRotation.RotateVector(Settings.LocalAttachOffset);
	FVector Torque = FVector::CrossProduct(AttachOffsetWorld, Force);

	FVector AngularAccel = Torque / Settings.RotationalInertia - AngularVelocity * Settings.RotDamping;
	AngularVelocity += AngularAccel * DeltaTime;

	float StepAngle = AngularVelocity.Size() * DeltaTime;

	if (StepAngle <= KINDA_SMALL_NUMBER)
	{
		return CurrentRotation;
	}

	FVector StepAxis = AngularVelocity / AngularVelocity.Size();
	FQuat DeltaRotation = FQuat(StepAxis, StepAngle);

	FQuat NewRotation = DeltaRotation * CurrentRotation;
	NewRotation.Normalize();

	return NewRotation;
}