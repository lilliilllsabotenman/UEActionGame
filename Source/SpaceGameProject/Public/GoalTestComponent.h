#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoalTestComponent.generated.h"

class UPrimitiveComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACEGAMEPROJECT_API UGoalTestComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGoalTestComponent();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnGoalOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};