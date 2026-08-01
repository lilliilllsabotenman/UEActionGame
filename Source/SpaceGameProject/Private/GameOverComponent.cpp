// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverComponent.h"
#include "MyCharacter.h"

// Sets default values for this component's properties
UGameOverComponent::UGameOverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGameOverComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AMyCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AddTickPrerequisiteActor(OwnerCharacter);
		OwnerCharacter->OnCharacterHit.AddUObject(this, &UGameOverComponent::HandleCharacterHit);
	}
}


// Called every frame
void UGameOverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerCharacter) return;

	if (!OwnerCharacter->IsWeightless() &&
		(OwnerCharacter->GetPlayerRopeState() == PlayerRopeState::Fall ||
		OwnerCharacter->GetPlayerRopeState() == PlayerRopeState::ChangeGravity))
	{
		elapsedTime += DeltaTime;

		if (elapsedTime >= TimeLimit)
		{
			OwnerCharacter->Death();
			elapsedTime = 0.f;
		}
	}
	else
	{
		elapsedTime = 0.f;
	}

	PrevVelocity = OwnerCharacter->GetVelocity();
}

void UGameOverComponent::HandleCharacterHit(const FHitResult& Hit, const FVector& Velocity)
{
	if (!OwnerCharacter) return;

	const float ImpactSpeed = FVector::DotProduct(PrevVelocity, -Hit.ImpactNormal);

	if (ImpactSpeed >= InpactVelocity)
	{
		OwnerCharacter->Death();
	}
}
