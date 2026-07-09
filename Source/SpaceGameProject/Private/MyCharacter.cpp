#include "MyCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"
#include "PlayerCameraComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"
#include "RopeSolverComponent.h"

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

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMyCharacter::BeginPlay()
{
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

	goalTrackerComponent = GetComponentByClass<UGoalTrackerComponent>();
	gameRule = GetComponentByClass<UGameRuleComponent>();

	GetWorldTimerManager().SetTimer(PlayerLocationBroadcastTimerHandle, this, &AMyCharacter::BroadcastPlayerLocation, PlayerLocationBroadcastInterval, true);
}

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

	for (TScriptInterface<ICharacterComponent>& Component : CharacterComponents)
	{
		Component->OnActionPassive();
	}
}

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnCharacterLanded.Broadcast(Hit);
}

void AMyCharacter::OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const FVector Velocity = GetCharacterMovement()->Velocity;

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
		FString::Printf(TEXT("[Broadcast] OnCharacterHit Velocity=%s ImpactNormal=%s"), *Velocity.ToString(), *Hit.ImpactNormal.ToString()));

	OnCharacterHit.Broadcast(Hit, Velocity);
}


void AMyCharacter::BroadcastPlayerLocation()
{
	OnPlayerLocationUpdated.Broadcast(GetActorLocation());
}

//=====================================

void AMyCharacter::GetItem(UItemKey* Key)
{
	// GravityComponent->AddResource(); 
}

void AMyCharacter::Death()
{

	onDeath();
}

void AMyCharacter::Goal()
{
   if(!gameRule->MissonCompleted()) return;
   onGoal();
}

PlayerRopeState AMyCharacter::GetPlayerRopeState() const
{
	if (URopeSolverComponent* Rope = FindComponentByClass<URopeSolverComponent>())
	{
		if (Rope->IsHooked())
		{
			return PlayerRopeState::Rope;
		}
	}

	return GetCharacterMovement()->IsFalling() ? PlayerRopeState::Fall : PlayerRopeState::Ground;
}