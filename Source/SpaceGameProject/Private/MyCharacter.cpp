#include "MyCharacter.h"

#include "ObjectTracker.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"
#include "PlayerCameraComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"

#pragma region コンストラクタ

AMyCharacter::AMyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMyMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	SpringArm = CreateDefaultSubobject<ULocalOffsetSpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UPlayerCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 初期値は地上仕様(true)。Rope中はUMyMovementComponent::TickComponentが毎ティックfalseに切り替える。
	GetCharacterMovement()->bOrientRotationToMovement = true;

	RopeStateDelegate.BindUObject(this, &AMyCharacter::GetPlayerRopeState);
}

#pragma endregion
#pragma region BeginPlay

void AMyCharacter::BeginPlay()
{
	// CreateDefaultSubobject(コンストラクタ+CDO経由)だと、Blueprintのコンパイル/ロード時に
	// CDOのObjectTrackerがnullになる問題があったため、BeginPlayでNewObjectして生成する。
	// ItemTrackerComponent等のBeginPlayはSuper::BeginPlay()の中で呼ばれるので、その前に生成必須。
	ObjectTracker = NewObject<UObjectTracker>(this, TEXT("ObjectTracker"));

	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnCapsuleHit);

	// 遮蔽物越しでもPlayerが見えるよう、アウトライン用ポストプロセスマテリアルの参照先としてCustomDepthを有効化
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(OutlineCustomDepthStencilValue);

	itemTrackerComponent = GetComponentByClass<UItemTrackerComponent>();
	gameRule = GetComponentByClass<UGameRuleComponent>();

	SetDefaultVelocity(1000);
}

#pragma endregion

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CharacterComponents.Empty();
	TArray<UActorComponent*> AllComponents;
	GetComponents(AllComponents);
	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp && Comp->Implements<UCharacterComponent>())
		{
			CharacterComponents.Add(TScriptInterface<ICharacterComponent>(Comp));
		}
	}

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		for (TScriptInterface<ICharacterComponent>& Component : CharacterComponents)
		{
			Component->BindInput(EnhancedInput);
		}
	}

	
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rope・ChangeGravity・AntiGravityは外部から明示的にセットされる状態のため、Tickでは上書きしない。
	if (CurrentRopeState != PlayerRopeState::Rope && CurrentRopeState != PlayerRopeState::ChangeGravity && CurrentRopeState != PlayerRopeState::AntiGravity)
	{
		CurrentRopeState = GetCharacterMovement()->IsFalling() ? PlayerRopeState::Fall : PlayerRopeState::Ground;
	}

	// RopeStateのモニタリング用。画面に現在の状態を表示する
	if (GEngine)
	{
		const UEnum* RopeStateEnum = StaticEnum<PlayerRopeState>();
		const FString RopeStateName = RopeStateEnum ? RopeStateEnum->GetNameStringByValue(static_cast<int64>(CurrentRopeState)) : TEXT("Unknown");
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Cyan, FString::Printf(TEXT("RopeState: %s"), *RopeStateName));
	}

	for (TScriptInterface<ICharacterComponent>& Component : CharacterComponents)
	{
		Component->OnActionPassive();
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if(PC)
	{	
		RectPlayerLocation = LocationCalucultior.CalculateBurstCenterUV(PC, GetActorLocation());
	}
}

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CurrentRopeState == PlayerRopeState::ChangeGravity)
	{
		SetPlayerRopeState(PlayerRopeState::Ground);
	}

	OnCharacterLanded.Broadcast(Hit);
}

void AMyCharacter::OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const FVector Velocity = GetCharacterMovement()->Velocity;
	OnCharacterHit.Broadcast(Hit, Velocity);
}

//=====================================

void AMyCharacter::GetItem()
{
	OnGetItem.Broadcast();
}

void AMyCharacter::Death()
{
	DeathPhysics();
	onDeath();
}

void AMyCharacter::DeathPhysics()
{
	GetMesh()->SetSimulatePhysics(true);
}

void AMyCharacter::Goal()
{
//    if(!gameRule->MissonCompleted()) return;
   onGoal();
}

PlayerRopeState AMyCharacter::GetPlayerRopeState() const
{
	return CurrentRopeState;
}

void AMyCharacter::SetPlayerRopeState(PlayerRopeState NewState)
{
	CurrentRopeState = NewState;
}

FGetRopeStateDelegate& AMyCharacter::GetRopeStateDelegate()
{
	return RopeStateDelegate;
}

void AMyCharacter::SetDefaultVelocity(float Speed)
{
	FVector VelocityVector = GetActorForwardVector();

	GetCharacterMovement()->Velocity = VelocityVector * Speed;
}