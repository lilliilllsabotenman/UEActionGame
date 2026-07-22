// HookGuideFinder.cpp

#include "HookGuideFinder.h"
#include "HookGuide.h"
#include "Engine/World.h"

FVector HookGuideFinder::ComputeConeRayDirection(const FVector& Forward, float ConeAngleDegrees, float AzimuthDegrees)
{
	const FRotationMatrix Basis(Forward.Rotation());
	const FVector Right = Basis.GetScaledAxis(EAxis::Y);
	const FVector Up = Basis.GetScaledAxis(EAxis::Z);

	const float ConeAngleRad = FMath::DegreesToRadians(ConeAngleDegrees);
	const float AzimuthRad = FMath::DegreesToRadians(AzimuthDegrees);

	const FVector OffsetDirection = Right * FMath::Cos(AzimuthRad) + Up * FMath::Sin(AzimuthRad);
	return (Forward * FMath::Cos(ConeAngleRad) + OffsetDirection * FMath::Sin(ConeAngleRad)).GetSafeNormal();
}

bool HookGuideFinder::FindHookTarget(
	UWorld* World,
	const FVector& RayOrigin,
	const FVector& ForwardDirection,
	AActor* IgnoreActor,
	const FRopeSettings& Settings,
	FVector& OutTargetPosition)
{
	if (!World) return false;

	FCollisionQueryParams Params;
	if (IgnoreActor)
	{
		Params.AddIgnoredActor(IgnoreActor);
	}

	const FVector Forward = ForwardDirection.GetSafeNormal();

	// 正面レイ
	FHitResult ForwardHit;
	const bool bForwardHit = World->LineTraceSingleByChannel(
		ForwardHit, RayOrigin, RayOrigin + Forward * Settings.MaxRopeLength, ECC_Visibility, Params);

	if (bForwardHit && Cast<AHookGuide>(ForwardHit.GetActor()))
	{
		OutTargetPosition = ForwardHit.GetActor()->GetActorLocation();
		return true;
	}

	// 正面が外れていても、周囲にHookGuideがあれば優先的にスナップする
	for (int32 Index = 0; Index < Settings.HookAssistRayCount; ++Index)
	{
		const float AzimuthDegrees = 360.f * Index / FMath::Max(Settings.HookAssistRayCount, 1);
		const FVector RayDirection = ComputeConeRayDirection(Forward, Settings.HookAssistSpreadAngle, AzimuthDegrees);

		FHitResult ScatterHit;
		if (World->LineTraceSingleByChannel(
			ScatterHit, RayOrigin, RayOrigin + RayDirection * Settings.MaxRopeLength, ECC_Visibility, Params))
		{
			if (AHookGuide* HookGuide = Cast<AHookGuide>(ScatterHit.GetActor()))
			{
				OutTargetPosition = HookGuide->GetActorLocation();
				return true;
			}
		}
	}

	// HookGuideは見つからなかった。正面レイの素のヒット点にフォールバック
	if (bForwardHit)
	{
		OutTargetPosition = ForwardHit.ImpactPoint;
		return true;
	}

	return false;
}
