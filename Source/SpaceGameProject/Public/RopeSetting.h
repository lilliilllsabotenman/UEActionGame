// RopeSettings.h

#pragma once

#include "CoreMinimal.h"
#include "RopeSetting.generated.h"

USTRUCT(BlueprintType)
struct SPACEGAMEPROJECT_API FRopeSettings
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float SpringK = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float Damping = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float ConstraintStrength = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float ReelSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float ReelRate = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float RopeLength = 1000.f;

	// Max trace distance when acquiring a hook target. RopeLength itself gets overwritten with the actual hit distance once hooked.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float MaxRopeLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float Gravity = -980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float SpringStrength = 50.f;
};