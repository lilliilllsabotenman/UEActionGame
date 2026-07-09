// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanterComponent.h"

// Sets default values for this component's properties
UPlanterComponent::UPlanterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlanterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...


	
}

void UPlanterComponent::PositionOffsetInitialized()//���������I�u�W�F�N�g������
{
    AActor* CDO = Cast<AActor>(plantObject->GetDefaultObject());
    UStaticMeshComponent* Mesh = CDO->GetComponentByClass<UStaticMeshComponent>();
    if (Mesh && Mesh->GetStaticMesh())
    {
        FBoxSphereBounds LocalBounds = Mesh->GetStaticMesh()->GetBounds();
        PlantPointOffset = LocalBounds.BoxExtent.Z + LocalBounds.Origin.Z; // �s�{�b�g�I�t�Z�b�g����
    }
}


// Called every frame
void UPlanterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Camera.IsValid()) return;
    if (_state == PlanterState::Free) return;

    ObjectLogationSettings(Camera.Get(), FVector::ZeroVector, FVector::ZeroVector);
    ObjectLocationUpdate();
}

void UPlanterComponent::SpawnObject()
{
    if (_state == PlanterState::Planting)
    {
        Planting();
        return;
    }

    if (!plantObject) return;

    CroneObject = GetWorld()->SpawnActor<APlantObject>(plantObject, objectLocatrion, objectRotation);
    _state = PlanterState::Planting;
}

template<typename T>
void UPlanterComponent::ObjectLogationSettings(T* Component, const FVector& Start, const FVector& End)//�n�ʔ�����擾���ăI�u�W�F�N�g�̈ʒu�Ɖ�]���`����
{
    FVector Origin = Component->GetComponentLocation();
    FVector Direction = Component->GetForwardVector();
    FVector RayEnd = Origin + Direction * 10000.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());
    Params.AddIgnoredActor(CroneObject);


    if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, RayEnd, ECC_Visibility, Params))
    {
        FVector normalVector = Hit.ImpactNormal;

        objectRotation = normalVector.Rotation();
        objectLocatrion = Hit.ImpactPoint;

    }
}

void UPlanterComponent::ObjectLocationUpdate()//��̊֐��łƂ������l�𔽉f����
{
    if (!CroneObject) return;
    CroneObject->SetActorLocation(objectLocatrion);
    CroneObject->SetActorRotation(objectRotation);
}

void UPlanterComponent::PlantCancel()
{
    if (!IsValid(CroneObject)) return;
    
    CroneObject->Destroy();
    CroneObject = nullptr;
    _state = PlanterState::Free;
}

void UPlanterComponent::Planting()
{
    CroneObject->Plant();
    _state = PlanterState::Free;

    OnObjectPlanted.Broadcast(CroneObject);
    CroneObject = nullptr;
}