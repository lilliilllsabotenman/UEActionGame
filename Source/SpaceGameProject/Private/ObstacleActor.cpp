// Fill out your copyright notice in the Description page of Project Settings.

#include "ObstacleActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AObstacleActor::AObstacleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Mesh;
	ProjectileMovement->ProjectileGravityScale = 1.f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
}

void AObstacleActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpanSeconds);
}

void AObstacleActor::Launch(const FVector& InVelocity)
{
	if (!ProjectileMovement) return;

	ProjectileMovement->Velocity = InVelocity;
}
