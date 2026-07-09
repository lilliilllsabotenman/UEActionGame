#include "LocalOffsetSpringArmComponent.h"
#include "GameFramework/Actor.h"

ULocalOffsetSpringArmComponent::ULocalOffsetSpringArmComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    // SocketOffsetはこのクラスが管理するのでBP/コンストラクタで触らない
    SocketOffset = FVector::ZeroVector;
}

void ULocalOffsetSpringArmComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    // 先にSpringArmの標準処理（アーム長・コリジョン補正等）を走らせる
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // ActorのLocal軸をWorld空間に展開してオフセット計算
    FVector WorldOffset =
        Owner->GetActorRightVector() * LocalOffset.Y +
        Owner->GetActorUpVector() * LocalOffset.Z +
        Owner->GetActorForwardVector() * LocalOffset.X;

    // SpringArmのWorld回転の逆でLocal空間に戻す
    // SocketOffsetはSpringArm先端のLocal空間で解釈されるため
    FVector SpringArmLocal = GetComponentTransform().InverseTransformVectorNoScale(WorldOffset);

    SocketOffset = SpringArmLocal;
}