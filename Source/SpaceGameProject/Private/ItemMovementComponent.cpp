// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemMovementComponent.h"
#include "ItemObjectActor.h"
#include "MyCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

// Sets default values for this component's properties
UItemMovementComponent::UItemMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UItemMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsurePlayerCharacterBound();
}

void UItemMovementComponent::EnsurePlayerCharacterBound()
{
	if (BoundPlayerCharacter.IsValid()) return;

	// Level-placed actors' BeginPlay order relative to the player pawn's possession isn't
	// guaranteed, so GetPlayerCharacter can still return null here. Keep retrying from Tick
	// until it succeeds instead of only trying once in BeginPlay.
	if (AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		BoundPlayerCharacter = PlayerCharacter;
		PlayerCharacter->OnPlayerLocationUpdated.AddUObject(this, &UItemMovementComponent::HandlePlayerLocationUpdated);
	}
}

void UItemMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AMyCharacter* PlayerCharacter = BoundPlayerCharacter.Get())
	{
		PlayerCharacter->OnPlayerLocationUpdated.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UItemMovementComponent::HandlePlayerLocationUpdated(const FVector& NewLocation)
{
	LastKnownPlayerLocation = NewLocation;
	bHasPlayerLocation = true;
}

void UItemMovementComponent::SetVelocity(FVector NewVelocity)
{
	Velocity = NewVelocity;
}

// Called every frame
void UItemMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	EnsurePlayerCharacterBound();

	AgeTimer += DeltaTime;

	if (!bIsStuck)
	{
		ApplyCohesion(DeltaTime);
		ApplyPlayerAttraction(DeltaTime);
		ApplyMovement(DeltaTime);
	}

	UpdateSquish(DeltaTime);
}

void UItemMovementComponent::ApplyCohesion(float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Owner);

	TArray<AActor*> NearbyItems;
	UKismetSystemLibrary::SphereOverlapActors(
		Owner,
		Owner->GetActorLocation(),
		CohesionRadius,
		ObjectTypes,
		AItemObjectActor::StaticClass(),
		IgnoreActors,
		NearbyItems);

	for (AActor* Other : NearbyItems)
	{
		if (!Other) continue;

		const FVector ToOther = Other->GetActorLocation() - Owner->GetActorLocation();
		const float Distance = ToOther.Size();
		if (Distance < KINDA_SMALL_NUMBER) continue;

		Velocity += (ToOther / Distance) * CohesionStrength * DeltaTime;
	}
}

void UItemMovementComponent::ApplyPlayerAttraction(float DeltaTime)
{
	if (!bHasPlayerLocation) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector ToPlayer = LastKnownPlayerLocation - Owner->GetActorLocation();
	Velocity += ToPlayer.GetSafeNormal() * PlayerAttractionStrength * DeltaTime;
}

void UItemMovementComponent::ApplyMovement(float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	Velocity.Z += GetWorld()->GetGravityZ() * GravityScale * DeltaTime;
	Velocity = FMath::VInterpTo(Velocity, FVector::ZeroVector, DeltaTime, VelocityDamping);

	FHitResult Hit;
	Owner->AddActorWorldOffset(Velocity * DeltaTime, true, &Hit);

	if (Hit.bBlockingHit && AgeTimer >= StuckGracePeriod)
	{
		bIsStuck = true;
		Velocity = FVector::ZeroVector;
		StartSquish(Hit.ImpactNormal);
	}
}

void UItemMovementComponent::StartSquish(const FVector& ImpactNormal)
{
	bIsSquishing = true;
	SquishTimer = 0.f;
	SquishAxis = ImpactNormal.GetSafeNormal();
	LastSquishMultiplier = FVector::OneVector;
}

void UItemMovementComponent::UpdateSquish(float DeltaTime)
{
	if (!bIsSquishing) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	SquishTimer += DeltaTime;
	const float Alpha = FMath::Clamp(SquishTimer / SquishDuration, 0.f, 1.f);
	const float CurrentSquish = FMath::Lerp(SquishAmount, 0.f, Alpha);

	const FVector AxisWeight = SquishAxis.GetAbs();
	const FVector FlattenFactor = FVector(1.f) - AxisWeight * CurrentSquish;
	const FVector StretchFactor = FVector(1.f) + (FVector(1.f) - AxisWeight) * (CurrentSquish * 0.5f);
	const FVector NewMultiplier = FlattenFactor * StretchFactor;

	// Undo last frame's contribution before applying this frame's, so this composes
	// correctly with AItemObjectActor's independent shrink-over-time scale changes.
	const FVector UnsquishedScale = Owner->GetActorScale3D() / LastSquishMultiplier;
	Owner->SetActorScale3D(UnsquishedScale * NewMultiplier);
	LastSquishMultiplier = NewMultiplier;

	if (Alpha >= 1.f)
	{
		bIsSquishing = false;
		LastSquishMultiplier = FVector::OneVector;
	}
}
