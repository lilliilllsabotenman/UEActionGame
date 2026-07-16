// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemProximityTrigger.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UItemProximityTrigger::HandlePlayerLocationUpdated(const FVector& NewLocation)
{
	AActor* own = GetOwner();
	AItemParent* itemParent = Cast<AItemParent>(own);

	if(!itemParent) return;

	if(Distance > FVector::Dist(own -> GetActorLocation(), NewLocation))
	{
		itemParent -> TriggerItemBurst();
	}
}

void UItemProximityTrigger::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if(!PlayerCharacter) return;

	UPlayerLocationBroadcasterComponent* broadcaster = PlayerCharacter -> FindComponentByClass<UPlayerLocationBroadcasterComponent>();

	if(!broadcaster) return;

	broadcaster -> OnPlayerLocationUpdated.AddUObject(this, &UItemProximityTrigger::HandlePlayerLocationUpdated);
}

void UItemProximityTrigger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

