#include "GoalTestComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"

UGoalTestComponent::UGoalTestComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGoalTestComponent::BeginPlay()
{
    Super::BeginPlay();

    if (UPrimitiveComponent* Primitive =
        Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
    {
        Primitive->OnComponentBeginOverlap.AddDynamic(
            this,
            &UGoalTestComponent::OnGoalOverlap);
    }
}

void UGoalTestComponent::OnGoalOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == GetOwner())
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Goal"));
}