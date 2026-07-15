// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemProximityTrigger.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void UItemProximityTrigger::HandlePlayerLocationUpdated(const FVector& NewLocation)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("!!!!!!"));
	AActor* own = GetOwner();
	AItemParent* itemParent = Cast<AItemParent>(own);

	if(!itemParent) return;

	if(Distance > FVector::Dist(own -> GetActorLocation(), NewLocation))
	{
		itemParent -> TriggerItemBurst();

		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("!!!!!!"));
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

