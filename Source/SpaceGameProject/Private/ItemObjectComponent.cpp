// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemObjectComponent.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UItemObjectComponent::UItemObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UItemObjectComponent::BeginPlay()
{
    Super::BeginPlay();

    // SphereComponent�𖼑O�Œ��ڎ擾
    UPrimitiveComponent* Collision = GetOwner()->FindComponentByClass<UStaticMeshComponent>();

    if (Collision)
    {
        Collision->OnComponentBeginOverlap.AddDynamic(this, &UItemObjectComponent::OnOverlapBegin);
    }
}


// Called every frame
void UItemObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UItemObjectComponent::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMyCharacter* MyChar = Cast<AMyCharacter>(OtherActor);
	if (!MyChar) return;

	MyChar->GetItem(Key);

	GetOwner()->Destroy();
}
