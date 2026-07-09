#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "LocalOffsetSpringArmComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEGAMEPROJECT_API ULocalOffsetSpringArmComponent : public USpringArmComponent
{
    GENERATED_BODY()

public:
    ULocalOffsetSpringArmComponent();

    // Actorローカル空間でのオフセット（Y=右肩, Z=高さ）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector LocalOffset = FVector(0.f, 60.f, 20.f);

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;
};