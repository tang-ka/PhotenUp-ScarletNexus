// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossCharacterBase.h"
#include "Boss/BossAIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Boss/BossAttackCollisionComponent.h"
 
ABossCharacterBase::ABossCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
 
	AIControllerClass = ABossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
 
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 500.f;
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
	}
 
	bUseControllerRotationYaw = false;
	
	// 오른발 콜리전
	RightFootCollision = CreateDefaultSubobject<UBossAttackCollisionComponent>(TEXT("RightFootCollision"));
	RightFootCollision->SetupAttachment(GetMesh(), FName("RightFoot"));
	RightFootCollision->SetSphereRadius(25.f);

	// 왼발 콜리전
	LeftFootCollision = CreateDefaultSubobject<UBossAttackCollisionComponent>(TEXT("LeftFootCollision"));
	LeftFootCollision->SetupAttachment(GetMesh(), FName("LeftFoot"));
	LeftFootCollision->SetSphereRadius(25.f);

	// 오른손 콜리전
	RightHandCollision = CreateDefaultSubobject<UBossAttackCollisionComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHand"));
	RightHandCollision->SetSphereRadius(20.f);

	// 왼손 콜리전
	LeftHandCollision = CreateDefaultSubobject<UBossAttackCollisionComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("LeftHand"));
	LeftHandCollision->SetSphereRadius(20.f);
}
 
void ABossCharacterBase::BeginPlay()
{
	Super::BeginPlay();
 
	if (BossConfig)
	{
		InitializeWithConfig(BossConfig);
	}
}
 
void ABossCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
 
 

// IDamageable 구현

 
bool ABossCharacterBase::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (IDamageable::Execute_IsDead(this)) return false;
 
	if (CurrentHPValue <= 0.f)
	{
		return false;
	}
 
	const float OldHP = CurrentHPValue;
	const float DamageAmount = static_cast<float>(DamageInfo.DamageAmount);
	CurrentHPValue = FMath::Clamp(CurrentHPValue - DamageAmount, 0.f, MaxHPValue);
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 대미지: %d (HP: %.0f -> %.0f / %.0f)"),
		DamageInfo.DamageAmount, OldHP, CurrentHPValue, MaxHPValue);
 
	OnHPChanged.Broadcast(CurrentHPValue, MaxHPValue, DamageAmount);
	CheckPhaseTransition();
 
	if (CurrentHPValue <= 0.f)
	{
		HandleDeath();
	}
 
	return true;
}
 
int ABossCharacterBase::GetHP_Implementation() const
{
	return static_cast<int>(CurrentHPValue);
}
 
float ABossCharacterBase::GetHPPercent_Implementation() const
{
	if (MaxHPValue <= 0.f) return 0.f;
	return FMath::Clamp(CurrentHPValue / MaxHPValue, 0.f, 1.f);
}
 
bool ABossCharacterBase::IsDead_Implementation() const
{
	return CurrentHPValue <= 0.f;
}
 
 

// ICombatState 구현

 
bool ABossCharacterBase::IsAttacking_Implementation() const
{
	return CurrentCombatState == EBossCombatState::MeleeAttack
		|| CurrentCombatState == EBossCombatState::RangedAttack
		|| CurrentCombatState == EBossCombatState::Teleport;
}
 
bool ABossCharacterBase::HasSuperArmor_Implementation() const
{
	return false;
}
 
 

// 보스 전용

 
void ABossCharacterBase::InitializeWithConfig(UBossConfigDataAsset* Config)
{
	if (!Config)
	{
		UE_LOG(LogTemp, Error, TEXT("[Boss] InitializeWithConfig: Config가 nullptr입니다."));
		return;
	}
 
	BossConfig = Config;
	MaxHPValue = Config->MaxHP;
	CurrentHPValue = MaxHPValue;
	CurrentPhase = EBossPhase::Phase1;
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 초기화 완료 - HP: %.0f"), MaxHPValue);
}
 
 

// 내부 메서드

 
void ABossCharacterBase::CheckPhaseTransition()
{
	if (!BossConfig) return;
 
	const float HPRatio = GetHPPercent_Implementation();
	EBossPhase NewPhase = CurrentPhase;
 
	if (HPRatio <= 0.25f)
		NewPhase = EBossPhase::Phase3_Cutscene;
	else if (HPRatio <= 0.50f)
		NewPhase = EBossPhase::Phase2_Enhanced;
	else if (HPRatio <= 0.70f)
		NewPhase = EBossPhase::Phase2;
 
	if (NewPhase != CurrentPhase)
	{
		const EBossPhase OldPhase = CurrentPhase;
		CurrentPhase = NewPhase;
 
		UE_LOG(LogTemp, Warning, TEXT("[Boss] ========== 페이즈 전환! =========="));
		UE_LOG(LogTemp, Warning, TEXT("[Boss] %d → %d (HP: %.0f / %.0f, 비율: %.1f%%)"),
			static_cast<int32>(OldPhase),
			static_cast<int32>(NewPhase),
			CurrentHPValue, MaxHPValue, HPRatio * 100.f);
 
		OnPhaseChanged.Broadcast(OldPhase, NewPhase);
 
		if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
		{
			BossAI->SendStateTreeEvent(
				FGameplayTag::RequestGameplayTag(FName("Boss.Event.PhaseTransition")));
		}
	}
}
 
void ABossCharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[Boss] 사망 처리 시작"));

	// 이벤트만 전송 — 나머지는 Death State에서 처리
	if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
	{
		BossAI->SendStateTreeEvent(
			FGameplayTag::RequestGameplayTag(FName("Boss.Event.Death")));
	}

	// ★ 여기서 AI 정지, 이동 정지, 콜리전 비활성화 하지 않기!
	// Death State의 EnterState에서 처리함
}