// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLocationCalculatior.h"

PlayerLocationCalculatior::PlayerLocationCalculatior()
{
}

PlayerLocationCalculatior::~PlayerLocationCalculatior()
{
}

FVector PlayerLocationCalculatior::CalculateBurstCenterUV(const APlayerController* PlayerController, const FVector& WorldPosition)
{
	FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector CameraForward = CameraRotation.Vector();
    FVector ToTarget = (WorldPosition - CameraLocation).GetSafeNormal();
    float DotResult = FVector::DotProduct(CameraForward, ToTarget);

    FVector2D ScreenPixelPosition;
    bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, WorldPosition, ScreenPixelPosition);

    int32 ViewportWidth;
    int32 ViewportHeight;
    PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

    if (DotResult <= 0.0f)
    {
        FVector2D ScreenCenter(ViewportWidth * 0.5f, ViewportHeight * 0.5f);
        FVector2D FlippedOffset = ScreenCenter - (ScreenPixelPosition - ScreenCenter);
        ScreenPixelPosition = FlippedOffset;
    }

    FVector2D UV;
    UV.X = ScreenPixelPosition.X / ViewportWidth;
    UV.Y = ScreenPixelPosition.Y / ViewportHeight;

    UV.X = FMath::Clamp(UV.X, 0.0f, 1.0f);
    UV.Y = FMath::Clamp(UV.Y, 0.0f, 1.0f);

    float ValidityFlag = 0.0f;
    if (bIsOnScreen)
    {
        ValidityFlag = 1.0f;
    }

    return FVector(UV.X, UV.Y, ValidityFlag);
}
