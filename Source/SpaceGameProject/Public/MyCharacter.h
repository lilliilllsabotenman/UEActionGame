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
#include "ItemTrackerComponent.h"

#include "MyMovementComponent.h"

#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterLanded, const FHitResult&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterHit, const FHitResult&, const FVector&);

enum class PlayerRopeState
{
	Rope,
	Ground,
	Fall,
	ChangeGravity
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
	UItemTrackerComponent* itemTrackerComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Rope")
	float GrappleTraceDistance = 10000.f;

	FVector HookTargetLocation = FVector::ZeroVector;

	UFUNCTION(BlueprintImplementableEvent)
	void onDeath();

	UFUNCTION(BlueprintImplementableEvent)
	void onGoal();

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	FOnCharacterLanded OnCharacterLanded;
	FOnCharacterHit OnCharacterHit;

	void GetItem(UItemKey* Key);
	void Death();
	void Goal();

	PlayerRopeState GetPlayerRopeState() const;
	void SetPlayerRopeState(PlayerRopeState NewState);

private:

	bool bHasGoalItem=false;

	PlayerRopeState CurrentRopeState = PlayerRopeState::Ground;

	UPROPERTY()
	TArray<TScriptInterface<ICharacterComponent>> CharacterComponents;

private:

	// 未使用(実装なし)。重力処理はUChangeGravityComponentに移行済み。
	// void UseGravity();
	// void ChangeGravityStart();
	// void ChangeGravityEnd();
	// FVector GetLookActorLocation(float TraceDistance);
	// void PlayerGravitySolver();

	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};