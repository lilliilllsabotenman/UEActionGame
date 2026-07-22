#pragma once

#include "CharacterComponent.h"
#include "CoreMinimal.h"
#include "UObject/ScriptInterface.h"
#include "GameFramework/Character.h"
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
class UObjectTracker;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterLanded, const FHitResult&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterHit, const FHitResult&, const FVector&);
DECLARE_MULTICAST_DELEGATE(FOnGetItem);


UENUM(BlueprintType)
enum class PlayerRopeState : uint8
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

	UPROPERTY()
	UMyMovementComponent* MovementComponent = nullptr;

	UPROPERTY()
	UGameRuleComponent* gameRule = nullptr;

	UPROPERTY()
	UItemTrackerComponent* itemTrackerComponent = nullptr;

	// 追跡対象のマーカー描画を担う汎用マネージャー。ItemTrackerComponentなど複数コンポーネントに配って使い回す
	// CreateDefaultSubobjectだとBlueprintのコンパイル/ロード時にCDOのこの値がnullになる問題があったため、
	// BeginPlayでNewObjectして生成する(コンストラクタでは生成しない)
	UPROPERTY()
	UObjectTracker* ObjectTracker = nullptr;

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
	FOnGetItem OnGetItem;

	void GetItem();
	void Death();
	void Goal();
	
	// Death時のメッシュ物理シミュレーションON(いったん内部実装、BPのonDeathと併用)
	void DeathPhysics();

	UFUNCTION(BlueprintPure, Category = "Rope")
	PlayerRopeState GetPlayerRopeState() const;

	void SetPlayerRopeState(PlayerRopeState NewState);

	UObjectTracker* GetObjectTracker() const { return ObjectTracker; }

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
