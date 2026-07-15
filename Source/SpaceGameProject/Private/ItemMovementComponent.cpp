// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemMovementComponent.h"
#include "ItemObjectActor.h"
#include "GameFramework/Character.h"
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

	EnsureLocationBroadcasterBound();
}

void UItemMovementComponent::EnsureLocationBroadcasterBound()
{
	if (BoundLocationBroadcaster.IsValid()) return;

	// Level-placed actors' BeginPlay order relative to the player pawn's possession isn't
	// guaranteed, so GetPlayerCharacter can still return null here. Keep retrying from Tick
	// until it succeeds instead of only trying once in BeginPlay.
	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (UPlayerLocationBroadcasterComponent* Broadcaster = PlayerCharacter->FindComponentByClass<UPlayerLocationBroadcasterComponent>())
		{
			BoundLocationBroadcaster = Broadcaster;
			Broadcaster->OnPlayerLocationUpdated.AddUObject(this, &UItemMovementComponent::HandlePlayerLocationUpdated);
		}
	}
}

void UItemMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UPlayerLocationBroadcasterComponent* Broadcaster = BoundLocationBroadcaster.Get())
	{
		Broadcaster->OnPlayerLocationUpdated.RemoveAll(this);
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

	EnsureLocationBroadcasterBound();

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

	// 経過時間でVelocityへの上書き度合いを強め、最終的には慣性を無視してプレイヤー方向へ完全収束させる
	const float RampAlpha = PlayerAttractionRampUpTime > KINDA_SMALL_NUMBER ? FMath::Clamp(AgeTimer / PlayerAttractionRampUpTime, 0.f, 1.f) : 1.f;
	const float CurrentStrength = FMath::Lerp(PlayerAttractionStrength, MaxPlayerAttractionStrength, RampAlpha);

	const FVector ToPlayer = LastKnownPlayerLocation - Owner->GetActorLocation();
	const FVector TargetVelocity = ToPlayer.GetSafeNormal() * CurrentStrength;
	Velocity = FMath::Lerp(Velocity, TargetVelocity, RampAlpha);
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
