#pragma once

#include "CharacterComponent.h"
#include "CoreMinimal.h"
#include "UObject/ScriptInterface.h"
#include "GameFramework/Character.h"
#include "ItemKey.h"
#include "ChangeGravityComponent.h"
#include "LocalOffsetSpringArmComponent.h"
#include "RotationCompositorComponent.h"
#include "GameRuleComponent.h"
#include "GoalTrackerComponent.h"

#include "MyMovementComponent.h"

#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterLanded, const FHitResult&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerLocationUpdated, const FVector&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterHit, const FHitResult&, const FVector&);

enum class PlayerRopeState
{
	Rope,
	Ground,
	Fall
};

UCLASS()
class SPACEGAMEPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter(const FObjectInitializer& ObjectInitializer);

	FVector LocalVectorUP;

protected:

	virtual void BeginPlay() override;
	
//=============Input==========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputMappingContext* DefaultMappingContext=nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction=nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction=nullptr;


//=============Camera=============

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* Camera=nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USpringArmComponent* SpringArm = nullptr;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnlym Category = "VALUE")

	USkeletalMeshComponent* Mesh = GetMesh();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemCategory")
	UItemKey* DefaultItem = nullptr;

	UPROPERTY()
	UMyMovementComponent* MovementComponent = nullptr;

	UPROPERTY()
	UGameRuleComponent* gameRule = nullptr;

	UPROPERTY()
	UGoalTrackerComponent* goalTrackerComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Rope")
	float GrappleTraceDistance = 10000.f;

	FVector HookTargetLocation = FVector::ZeroVector;

	UFUNCTION(BlueprintImplementableEvent)
	void onDeath();

	UFUNCTION(BlueprintImplementableEvent)
	void onGoal();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tracking")
	float PlayerLocationBroadcastInterval = 0.2f;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	FOnCharacterLanded OnCharacterLanded;
	FOnPlayerLocationUpdated OnPlayerLocationUpdated;
	FOnCharacterHit OnCharacterHit;

	void GetItem(UItemKey* Key);
	void Death();
	void Goal();

	PlayerRopeState GetPlayerRopeState() const;

private:

	bool bHasGoalItem=false;

	FTimerHandle PlayerLocationBroadcastTimerHandle;

	UPROPERTY()
	TArray<TScriptInterface<ICharacterComponent>> CharacterComponents;

private:

	void UseGravity();

	void ChangeGravityStart();
	void ChangeGravityEnd();

	FVector GetLookActorLocation(float TraceDistance);
	void PlayerGravitySolver();
	void BroadcastPlayerLocation();

	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};