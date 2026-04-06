// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/RotationMatrix.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ScarletNexus.h"
#include "Components/BoxComponent.h"
#include "Data/ComboAttackDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/DamageableHelper.h"
#include "Player/Animation/KasaneAnimInstance.h"
#include "Player/Component/ActionManagerComponent.h"
#include "Player/Component/ComboComponent.h"
#include "Player/Component/InputBufferComponent.h"
#include "Player/Component/PlayerCharacterMovementComponent.h"
#include "Player/Component/PlayerPerceptionComponent.h"
#include "Player/Component/PlayerStateComponent.h"
#include "Player/Component/PlayerStatsComponent.h"
#include "Player/Component/PsychokinesisComponent.h"
#include "Player/Component/DashSkillComponent.h"

APlayerCharacterBase::APlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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
	SpringArmComp->TargetArmLength = 700.0f;
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 70.0f);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 5.0f;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 900.0f, 0.0f);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	GetCharacterMovement()->JumpZVelocity = 600.f;

	StatsComp = CreateDefaultSubobject<UPlayerStatsComponent>(TEXT("StatsComp"));
	StateComp = CreateDefaultSubobject<UPlayerStateComponent>(TEXT("StateComp"));
	PerceptionComp = CreateDefaultSubobject<UPlayerPerceptionComponent>(TEXT("PerceptionComp"));
	PsychokinesisComp = CreateDefaultSubobject<UPsychokinesisComponent>(TEXT("PsychokinesisComp"));
	InputBufferComp = CreateDefaultSubobject<UInputBufferComponent>(TEXT("InputBufferComp"));
	ActionManagerComp = CreateDefaultSubobject<UActionManagerComponent>(TEXT("ActionManagerComp"));
	ComboComp = CreateDefaultSubobject<UComboComponent>(TEXT("ComboComp"));
	DashSkillComp = CreateDefaultSubobject<UDashSkillComponent>(TEXT("DashSkillComp"));
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// StatsComp 죽음 델리게이트 바인딩
	StatsComp->OnDeath.AddUObject(this, &APlayerCharacterBase::HandleDeath);

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

	auto* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageStarted.AddDynamic(this, &APlayerCharacterBase::OnMontageStarted);
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacterBase::OnMontageEnded);
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if (StatsComp->IsDead())
	{
		return false;
	}

	StatsComp->ReceiveDamage(DamageInfo.DamageAmount);
	return true;
}

int APlayerCharacterBase::GetHP_Implementation() const
{
	return StatsComp->GetCurrentHP();
}

float APlayerCharacterBase::GetHPPercent_Implementation() const
{
	return StatsComp->GetHPPercentage();
}

bool APlayerCharacterBase::IsDead_Implementation() const
{
	return StatsComp->IsDead();
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
	FVector NextDashDirection = GetLastMovementInputVector();
	bool bAdjustLook = false;

	if (NextDashDirection.IsNearlyZero())
	{
		NextDashDirection = CameraComp->GetForwardVector();
		bAdjustLook = true;
	}

	DashSkillComp->StartDash(NextDashDirection, bAdjustLook);
}

void APlayerCharacterBase::OnBasicAttackInput(const FInputActionValue& Value)
{
	BasicAttack();
}

void APlayerCharacterBase::OnPsychokinesisInput(const FInputActionValue& Value)
{
	PsychicAttack();
}

void APlayerCharacterBase::OnCompletePsychokinesisInput(const FInputActionValue& Value)
{
	PsychokinesisComp->ReleaseHold();
}

void APlayerCharacterBase::OnBackAttackInput(const FInputActionValue& Value)
{
	BackStepAttack();
}

void APlayerCharacterBase::OnLockOnInput(const FInputActionValue& Value)
{
	LockOnToggle();
}
#pragma endregion

void APlayerCharacterBase::BasicAttack()
{
	if (ExecuteAttack(EAttackType::BasicAttack))
	{
		// 공격이 실제로 실행된 경우에만 타겟 방향으로 회전
		if (PerceptionComp)
		{
			if (AActor* CurrentTarget = PerceptionComp->GetCurrentTarget())
			{
				FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
				ToTarget.Z = 0.f;
				if (!ToTarget.IsNearlyZero())
				{
					SetActorRotation(ToTarget.GetSafeNormal().Rotation());
				}
			}
		}
	}
}

void APlayerCharacterBase::PsychicAttack()
{
	ExecuteAttack(EAttackType::PsychicAttack);

	PsychokinesisComp->SetTarget(GetPerceptionComp()->GetCurrentTarget());
	PsychokinesisComp->SetPickedObject(GetPerceptionComp()->GetPsychokinesisTarget());
	PsychokinesisComp->StartHold();
}

void APlayerCharacterBase::BackStepAttack()
{
	ExecuteAttack(EAttackType::BackStepAttack);

	FVector NextDashDirection = -GetCameraComp()->GetForwardVector();
	DashSkillComp->StartDash(NextDashDirection, true);
}

bool APlayerCharacterBase::ExecuteAttack(EAttackType AttackType)
{
	// 1. 버퍼에 입력 저장
	GetInputBufferComp()->BufferInput(AttackType);

	// 2. 공격 가능 여부 확인
	if (!GetActionManagerComp()->CanAttack())
	{
		PRINTLOG_SH(TEXT("공격 불가 상태"));
		return false;
	}

	// 3. 버퍼에서 입력 소비 시도 
	EAttackType NextAttackType;
	if (!GetInputBufferComp()->ConsumeBufferedInput(NextAttackType))
	{
		PRINTLOG_SH(TEXT("버퍼에 유효한 입력 없음"));
		return false;
	}

	// 4. 콤보 진행 시도
	if (!GetComboComp()->TryExecuteCombo(NextAttackType))
	{
		return false;
	}

	// 5. 상태 변경
	GetActionManagerComp()->TrySetState(EActionState::Attacking);

	// 6. 애니메이션 재생
	PlayCurrentAttackMontage();

	return true;
}

void APlayerCharacterBase::PlayCurrentAttackMontage()
{
	if (const UComboAttackDataAsset* CurrentAttack = GetComboComp()->GetCurrentAttack())
	{
		PlayAttackMontage(CurrentAttack);
	}
}

void APlayerCharacterBase::PlayAttackMontage(const UComboAttackDataAsset* AttackDataAsset)
{
	if (!AttackDataAsset || !AttackDataAsset->AttackMontage)
	{
		PRINTLOG_SH(TEXT("공격 데이터 에셋 또는 몽타주 없음"));
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		PRINTLOG_SH(TEXT("애니메이션 인스턴스 없음"));
		return;
	}

	PlayAnimMontage(AttackDataAsset->AttackMontage,
	                AttackDataAsset->MontagePlayRate,
	                AttackDataAsset->MontageSectionName);
	
	GetActionManagerComp()->SetMovementLocked(true);

	if (UKasaneAnimInstance* KasaneAnim = Cast<UKasaneAnimInstance>(AnimInstance))
	{
		KasaneAnim->SetIsBasicAttacking(true);
		KasaneAnim->SetAttackState(AttackDataAsset->AnimAttackState);
	}
}

void APlayerCharacterBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UKasaneAnimInstance* AnimInstance = Cast<UKasaneAnimInstance>(GetMesh()->GetAnimInstance());

	if (bInterrupted)
	{
		return;
	}
	
	if (ActionManagerComp->GetCurrentState() != EActionState::Attacking)
	{
		return;
	}

	ComboComp->ResetCombo();
	ActionManagerComp->ForceSetState(EActionState::Idle);

	// 공격 종료 시 이동 잠금 해제
	GetActionManagerComp()->SetMovementLocked(false);
	AnimInstance->SetAttackState(EAttackState::None);
}

void APlayerCharacterBase::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (UKasaneAnimInstance* AnimInstance = Cast<UKasaneAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetSpeed(GetVelocity().Size2D());

		const bool bInAir = GetCharacterMovement()->IsFalling();
		const bool bFalling = bInAir && GetVelocity().Z < 0.f;

		AnimInstance->SetIsInAir(bInAir);
		AnimInstance->SetIsFalling(bFalling);
		
		// PRINTLOG_SH(TEXT("IsInAir: %d, IsFalling: %d, IsJumpEnd: %d"), bInAir, bFalling, AnimInstance->IsJumEnd());
	}
}

void APlayerCharacterBase::HandleDeath()
{
	// 입력 비활성화
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	// 이동 중지
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 충돌 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PRINTLOG_SH(TEXT("플레이어 사망"));
}

void APlayerCharacterBase::TryConsumeBufferedAttack()
{
	if (!ActionManagerComp->IsComboWindowOpen())
	{
		return;
	}
	
	EAttackType BufferedType;
	if (!InputBufferComp->ConsumeBufferedInput(BufferedType))
	{
		return;
	}
	
	if (!GetComboComp()->TryExecuteCombo(BufferedType))
	{
		return;
	}

	ActionManagerComp->TrySetState(EActionState::Attacking);
	PlayCurrentAttackMontage();
}

void APlayerCharacterBase::Move(const FVector2D& InDirection)
{
	if (Controller == nullptr)
	{
		return;
	}

	// SetMovementLocked(true) 상태에서 입력 차단
	// (MOVE_None만으로는 UE5 CMC 파이프라인 전체를 막지 못함)
	if (ActionManagerComp->IsMovementLocked())
	{
		return;
	}

	// if (!ActionManagerComp->CanMove())
	// {
	// 	return;
	// }

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

void APlayerCharacterBase::LockOnToggle()
{
	auto* Perception = GetPerceptionComp();
	Perception->IsLockedOnActivate() ? Perception->DeactivateLockOn() : Perception->ActivateLockOn();
	UE_LOG(LogTemp, Warning, TEXT("%s"), Perception->IsLockedOnActivate() ? TEXT("Locked On") : TEXT("Locked Off"));
}
