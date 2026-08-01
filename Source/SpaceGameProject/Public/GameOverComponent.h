// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameOverComponent.generated.h"

class AMyCharacter;
struct FHitResult;

// 死亡条件(落下時間・衝突など)をここに集約する
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEGAMEPROJECT_API UGameOverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGameOverComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TimeSettings")
	float TimeLimit = 5.f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InpactVelocity")
	float InpactVelocity = 0.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY()
	AMyCharacter* OwnerCharacter = nullptr;

	float elapsedTime = 0.f;

	// 衝突処理で書き換えられる前の速度(Hit直前の近似値)
	FVector PrevVelocity = FVector::ZeroVector;

	void FallTimeCount(float dletaTime);

	// 衝突による死亡条件は未実装(判定基準を検討中)
	void HandleCharacterHit(const FHitResult& Hit, const FVector& Velocity);

};
