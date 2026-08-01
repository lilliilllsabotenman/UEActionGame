// Fill out your copyright notice in the Description page of Project Settings.

#include "AntiGravityFloor.h"
#include "ChangeGravityComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AAntiGravityFloor::AAntiGravityFloor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetRelativeScale3D(FVector(5.f, 5.f, 1.f));
	Mesh->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMeshFinder.Object);
	}

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(Mesh);
	TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECC_WorldStatic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AAntiGravityFloor::OnTriggerBeginOverlap);
}

void AAntiGravityFloor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMaterial();
}

void AAntiGravityFloor::UpdateMaterial()
{
	if (!Mesh) return;

	UMaterialInterface* TargetMaterial = IsAntiGravity ? AntiGravityMaterial : GravityMaterial;
	if (TargetMaterial)
	{
		Mesh->SetMaterial(0, TargetMaterial);
	}
}

void AAntiGravityFloor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UChangeGravityComponent* GravityComponent = OtherActor ? OtherActor->FindComponentByClass<UChangeGravityComponent>() : nullptr)
	{
		GravityComponent->NotifyAntiGravityFloorEntered(this);
	}
}
