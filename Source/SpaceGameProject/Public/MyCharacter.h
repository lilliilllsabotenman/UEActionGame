#pragma once

#include "PlayerLocationCalculatior.h"
#include "CharacterComponent.h"
#include "CoreMinimal.h"
#include "UObject/ScriptInterface.h"
#include "GameFramework/Character.h"
#include "ChangeGravityComponent.h"
#include "LocalOffsetSpringArmComponent.h"
#include "RotationCompositorComponent.h"
#include "GameRuleComponent.h"
#include "ItemTrackerComponent.h"
#include "StateObserver.h"

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
	ChangeGravity,
	AntiGravity
};

UCLASS()
class SPACEGAMEPROJECT_API AMyCharacter : public ACharacter, public IStateObserver
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector RectPlayerLocation = FVector::ZeroVector;

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

	// 遮蔽物越しアウトライン用。ポストプロセスマテリアル側でこのスタンシル値を参照する
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	int32 OutlineCustomDepthStencilValue = 1;

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

	// IStateObserver: ComponentがRopeStateを任意タイミングでPullするためのデリゲートを返す
	virtual FGetRopeStateDelegate& GetRopeStateDelegate() override;

	// PlayerRopeStateとは独立した無重力フラグ。Rope使用等でRopeStateが変わっても無重力状態を維持するために使う。
	UFUNCTION(BlueprintPure, Category = "Rope")
	bool IsWeightless() const { return bIsWeightless; }

	void SetWeightless(bool bNewWeightless) { bIsWeightless = bNewWeightless; }

	UObjectTracker* GetObjectTracker() const { return ObjectTracker; }

private:

	bool bHasGoalItem=false;

	PlayerRopeState CurrentRopeState = PlayerRopeState::Ground;

	FGetRopeStateDelegate RopeStateDelegate;

	bool bIsWeightless = false;

	UPROPERTY()
	TArray<TScriptInterface<ICharacterComponent>> CharacterComponents;

private:

	// 未使用(実装なし)。重力処理はUChangeGravityComponentに移行済み。
	// void UseGravity();
	// void ChangeGravityStart();
	// void ChangeGravityEnd();
	// FVector GetLookActorLocation(float TraceDistance);
	// void PlayerGravitySolver();

	PlayerLocationCalculatior LocationCalucultior;

	void SetDefaultVelocity(float Speed);

	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
