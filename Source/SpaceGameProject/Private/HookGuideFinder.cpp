// HookGuideFinder.cpp

#include "HookGuideFinder.h"
#include "HookGuide.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

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

	// 正面が外れていても、円錐角内かつ視線の通るHookGuideのうち正面に一番近いものにスナップする。
	// ただしあくまでアシストなので、正面レイが手前の何かに当たっている場合はその距離より遠いGuideには割り込ませない。
	TArray<AActor*> AllGuides;
	UGameplayStatics::GetAllActorsOfClass(World, AHookGuide::StaticClass(), AllGuides);

	const float MaxAngleRad = FMath::DegreesToRadians(Settings.HookAssistSpreadAngle);
	const float MaxCandidateDistance = bForwardHit
		? FVector::Dist(RayOrigin, ForwardHit.ImpactPoint)
		: Settings.MaxRopeLength;

	AActor* BestGuide = nullptr;
	float BestAngleRad = MaxAngleRad;

	for (AActor* GuideActor : AllGuides)
	{
		if (!GuideActor || GuideActor == IgnoreActor) continue;

		const FVector ToGuide = GuideActor->GetActorLocation() - RayOrigin;
		const float Distance = ToGuide.Size();
		if (Distance < KINDA_SMALL_NUMBER || Distance > MaxCandidateDistance) continue;

		const FVector Direction = ToGuide / Distance;
		const float AngleRad = FMath::Acos(FMath::Clamp(FVector::DotProduct(Forward, Direction), -1.f, 1.f));
		if (AngleRad > BestAngleRad) continue;

		// 障害物に遮られていないか確認する(対象自身に当たった場合はOK)
		FHitResult VisibilityHit;
		const bool bBlocked = World->LineTraceSingleByChannel(
			VisibilityHit, RayOrigin, GuideActor->GetActorLocation(), ECC_Visibility, Params);
		if (bBlocked && VisibilityHit.GetActor() != GuideActor) continue;

		BestGuide = GuideActor;
		BestAngleRad = AngleRad;
	}

	if (BestGuide)
	{
		OutTargetPosition = BestGuide->GetActorLocation();
		return true;
	}

	// HookGuideは見つからなかった。正面レイの素のヒット点にフォールバック
	if (bForwardHit)
	{
		OutTargetPosition = ForwardHit.ImpactPoint;
		return true;
	}

	return false;
}
