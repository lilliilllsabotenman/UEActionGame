#include "GravityController.h"
#include "GameFramework/Character.h"

void AGravityController::UpdateRotation(float DeltaTime)
{
	FVector GravityDirection = FVector::DownVector;

	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		GravityDirection = -PlayerCharacter->GetActorUpVector();
	}

	SmoothedGravityDirection = FMath::VInterpNormalRotationTo(
		SmoothedGravityDirection,
		GravityDirection,
		DeltaTime,
		GravityEaseSpeed);

	FRotator ViewRotation = GetControlRotation();


	ViewRotation =
		GetGravityRelativeRotation(
			ViewRotation,
			SmoothedGravityDirection);

	ViewRotation.Roll = 0;

	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(
			DeltaTime,
			ViewRotation,
			DeltaRot);

		ViewRotation.Roll = 0;

		SetControlRotation(GetGravityWorldRotation(ViewRotation, SmoothedGravityDirection));
	}

	APawn* const P = GetPawnOrSpectator();

	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}

FRotator AGravityController::GetGravityRelativeRotation(
	FRotator Rotation,
	FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);
		FRotator Result = (GravityRotation * Rotation.Quaternion()).Rotator(); 
		return Result;
	}

	return Rotation;
}

FRotator AGravityController::GetGravityWorldRotation(
	FRotator Rotation,
	FVector GravityDirection)
{


	if (!GravityDirection.Equals(FVector::DownVector))
	{
		FQuat GravityRotation = FQuat::FindBetweenNormals(FVector::DownVector, GravityDirection);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}
	return Rotation;
}