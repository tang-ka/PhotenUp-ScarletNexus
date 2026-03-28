// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/RotationMatrix.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/DamageableHelper.h"
#include "Player/Component/ActionManagerComponent.h"
#include "Player/Component/ComboComponent.h"
#include "Player/Component/InputBufferComponent.h"
#include "Player/Component/PlayerPerceptionComponent.h"
#include "Player/Component/PlayerStateComponent.h"
#include "Player/Component/PlayerStatsComponent.h"
#include "Player/Component/PsychokinesisComponent.h"

APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

#pragma region Input Action Initialization
	ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_PlayerAsset(
		TEXT("'/Game/SSH/Inputs/IMC_Player.IMC_Player'")
	);
	if (IMC_PlayerAsset.Succeeded())
	{
		IMC_Player = IMC_PlayerAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveAsset(
		TEXT("'/Game/SSH/Inputs/IA_Move.IA_Move'")
	);
	if (IA_MoveAsset.Succeeded())
	{
		IA_Move = IA_MoveAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> IA_LookAsset(
		TEXT("'/Game/SSH/Inputs/IA_Look.IA_Look'")
	);
	if (IA_LookAsset.Succeeded())
	{
		IA_Look = IA_LookAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> IA_JumpAsset(
		TEXT("'/Game/SSH/Inputs/IA_Jump.IA_Jump'")
	);
	if (IA_JumpAsset.Succeeded())
	{
		IA_Jump = IA_JumpAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> IA_DodgeAsset(
		TEXT("'/Game/SSH/Inputs/IA_Dodge.IA_Dodge'")
	);
	if (IA_DodgeAsset.Succeeded())
	{
		IA_Dodge = IA_DodgeAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> IA_BasicAttackAsset(
		TEXT("'/Game/SSH/Inputs/IA_BasicAttack.IA_BasicAttack'")
	);
	if (IA_BasicAttackAsset.Succeeded())
	{
		IA_BasicAttack = IA_BasicAttackAsset.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> IA_PsychokinesisAsset(
		TEXT("'/Game/SSH/Inputs/IA_Psychokinesis.IA_Psychokinesis'")
	);
	if (IA_PsychokinesisAsset.Succeeded())
	{
		IA_Psychokinesis = IA_PsychokinesisAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> IA_BackAttackAsset(
		TEXT("'/Game/SSH/Inputs/IA_BackAttack.IA_BackAttack'")
	);
	if (IA_BackAttackAsset.Succeeded())
	{
		IA_BackAttack = IA_BackAttackAsset.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> IA_LockOnAsset(
		TEXT("'/Game/SSH/Inputs/IA_LockOn.IA_LockOn'")
	);
	if (IA_LockOnAsset.Succeeded())
	{
		IA_LockOn = IA_LockOnAsset.Object;
	}
#pragma endregion

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 30.0f);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 5.0f;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 900.0f, 0.0f);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	StatsComp = CreateDefaultSubobject<UPlayerStatsComponent>(TEXT("StatsComp"));
	StateComp = CreateDefaultSubobject<UPlayerStateComponent>(TEXT("StateComp"));
	PerceptionComp = CreateDefaultSubobject<UPlayerPerceptionComponent>(TEXT("PerceptionComp"));
	PsychokinesisComp = CreateDefaultSubobject<UPsychokinesisComponent>(TEXT("PsychokinesisComp"));
	InputBufferComp = CreateDefaultSubobject<UInputBufferComponent>(TEXT("InputBufferComp"));
	ActionManagerComp = CreateDefaultSubobject<UActionManagerComponent>(TEXT("ActionManagerComp"));
	ComboComp = CreateDefaultSubobject<UComboComponent>(TEXT("ComboComp"));
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Add the input mapping context
	auto* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(IMC_Player, 0);
		}
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDashing)
	{
		DashTimeRemaining -= DeltaTime;

		if (DashTimeRemaining <= 0.f)
		{
			bIsDashing = false;

			// 대쉬 직후 속도 감쇠 (선택)
			GetCharacterMovement()->Velocity *= DashDampingFactor;

			GetWorldTimerManager().SetTimer(
				DashCooldownTimer,
				this,
				&APlayerCharacterBase::ResetDash,
				DashCooldown,
				false
			);
		}
		else
		{
			// 대쉬 속도 강제 적용
			GetCharacterMovement()->Velocity = DashVelocity;
		}
	}
	
	if (bNeedAdjustLookForward)
	{
		const FQuat CurQuat = GetActorQuat();
		// const FQuat TargetQuat = DashDirection.ToOrientationQuat();
		FVector CameraForwardXYPlane = GetCameraComp()->GetForwardVector();
		CameraForwardXYPlane.Z = 0.f;
		CameraForwardXYPlane.Normalize();
		const FQuat TargetQuat = CameraForwardXYPlane.ToOrientationQuat();
		const FQuat NewQuat = FQuat::Slerp(CurQuat, TargetQuat, 0.5f);
		SetActorRotation(NewQuat);
		if (FQuat::ErrorAutoNormalize(NewQuat, TargetQuat) < 0.01f)
		{
			SetActorRotation(TargetQuat);
			bNeedAdjustLookForward = false;
		}
	}
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* InputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputComp)
	{
		// Movement
		InputComp->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacterBase::OnMoveInput);
		InputComp->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacterBase::OnLookInput);
		InputComp->BindAction(IA_Jump, ETriggerEvent::Started, this, &APlayerCharacterBase::OnJumpInput);
		InputComp->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerCharacterBase::OnCompleteJumpInput);
		InputComp->BindAction(IA_Dodge, ETriggerEvent::Started, this, &APlayerCharacterBase::OnDodgeInput);

		// Skills
		InputComp->BindAction(IA_BasicAttack, ETriggerEvent::Started, this, &APlayerCharacterBase::OnBasicAttackInput);
		InputComp->BindAction(IA_Psychokinesis, ETriggerEvent::Started, this, &APlayerCharacterBase::OnPsychokinesisInput);
		InputComp->BindAction(IA_Psychokinesis, ETriggerEvent::Completed, this, &APlayerCharacterBase::OnCompletePsychokinesisInput);
		
		InputComp->BindAction(IA_BackAttack, ETriggerEvent::Started, this, &APlayerCharacterBase::OnBackAttackInput);
		InputComp->BindAction(IA_LockOn, ETriggerEvent::Started, this, &APlayerCharacterBase::OnLockOnInput);
	}
}

#pragma region IDamageable Interface
bool APlayerCharacterBase::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	return IDamageable::ReceiveDamage_Implementation(DamageInfo);
}

int APlayerCharacterBase::GetHP_Implementation() const
{
	return IDamageable::GetHP_Implementation();
}

float APlayerCharacterBase::GetHPPercent_Implementation() const
{
	return IDamageable::GetHPPercent_Implementation();
}

bool APlayerCharacterBase::IsDead_Implementation() const
{
	return IDamageable::IsDead_Implementation();
}
#pragma endregion

#pragma region Input Action Functions
void APlayerCharacterBase::OnMoveInput(const FInputActionValue& Value)
{
	const FVector2D InputDir = Value.Get<FVector2D>();
	Move(InputDir);
}

void APlayerCharacterBase::OnLookInput(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	Look(LookVector);
}

void APlayerCharacterBase::OnJumpInput(const FInputActionValue& Value)
{
	ACharacter::Jump();
}

void APlayerCharacterBase::OnCompleteJumpInput(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacterBase::OnDodgeInput(const FInputActionValue& Value)
{
	DashDirection = GetLastMovementInputVector();
	
	if (DashDirection.IsNearlyZero())
	{
		// DodgeDirection = GetMesh()->GetRightVector();
		// Camera 방향으로 대쉬하도록 변경
		DashDirection = CameraComp->GetForwardVector();
		bNeedAdjustLookForward = true;
	}
	
	Dash(DashDirection);
}

void APlayerCharacterBase::OnBasicAttackInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Basic Attack!"));
	BasicAttack();
}

void APlayerCharacterBase::OnPsychokinesisInput(const FInputActionValue& Value)
{
	PsychokinesisComp->SetTarget(GetPerceptionComp()->GetCurrentTarget());
	PsychokinesisComp->SetPickedObject(GetPerceptionComp()->GetPsychokinesisTarget());
	
	PsychokinesisComp->StartHold();
}

void APlayerCharacterBase::OnCompletePsychokinesisInput(const FInputActionValue& Value)
{
	PsychokinesisComp->ReleaseHold();
}

void APlayerCharacterBase::OnBackAttackInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Back Attack!"));
	BackAttack();
}

void APlayerCharacterBase::OnLockOnInput(const FInputActionValue& Value)
{
	LockOnToggle();
}
#pragma endregion

void APlayerCharacterBase::BackAttack()
{
	DashDirection = -GetCameraComp()->GetForwardVector();
	bNeedAdjustLookForward = true;
	Dash(DashDirection);
}

void APlayerCharacterBase::Move(const FVector2D& InDirection)
{
	if (Controller == nullptr)
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const FVector FinalDirection = Forward * InDirection.X + Right * InDirection.Y;

	AddMovementInput(FinalDirection);
}

void APlayerCharacterBase::Look(const FVector2D& LookVector)
{
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(-LookVector.Y);
}

void APlayerCharacterBase::Dash(FVector& InDashDirection)
{
	if (!bCanDash || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	bIsDashing = true;
	bCanDash = false;
	
	InDashDirection.Normalize();
	InDashDirection.Z = 0.f;

	DashVelocity = InDashDirection * (DashDistance / DashDuration);
	DashTimeRemaining = DashDuration;

	GetCharacterMovement()->Velocity.Z = 0;
}

void APlayerCharacterBase::ResetDash()
{
	bIsDashing = false;
	bCanDash = true;
}

void APlayerCharacterBase::LockOnToggle()
{
	auto* Perception = GetPerceptionComp();
	Perception->IsLockedOnActivate() ? Perception->DeactivateLockOn() : Perception->ActivateLockOn();
	UE_LOG(LogTemp, Warning, TEXT("%s"), Perception->IsLockedOnActivate() ? TEXT("Locked On") : TEXT("Locked Off"));
}
