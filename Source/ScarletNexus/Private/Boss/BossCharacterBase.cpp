// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossCharacterBase.h"
#include "Boss/BossAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
 
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
	UpdateStaggerDecay(DeltaTime);
}
 
// ============================================================
// IDamageable 구현 (데미지 + HP 조회)
// ============================================================
 
bool ABossCharacterBase::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
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
 
// ============================================================
// IStaggerable 구현
// ============================================================
 
float ABossCharacterBase::GetStaggerRatio_Implementation() const
{
	if (MaxStaggerGauge <= 0.f) return 0.f;
	return FMath::Clamp(CurrentStaggerGauge / MaxStaggerGauge, 0.f, 1.f);
}
 
void ABossCharacterBase::ApplyStaggerDamage_Implementation(float StaggerAmount)
{
	if (bSuperArmor)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Boss] 슈퍼아머 활성 - 경직 대미지 무시"));
		return;
	}
 
	const float OldStagger = CurrentStaggerGauge;
	CurrentStaggerGauge = FMath::Clamp(CurrentStaggerGauge + StaggerAmount, 0.f, MaxStaggerGauge);
	LastStaggerHitTime = GetWorld()->GetTimeSeconds();
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 경직 대미지: %.0f (Stagger: %.0f -> %.0f / %.0f)"),
		StaggerAmount, OldStagger, CurrentStaggerGauge, MaxStaggerGauge);
 
	OnStaggerChanged.Broadcast(CurrentStaggerGauge, MaxStaggerGauge);
 
	if (CurrentStaggerGauge >= MaxStaggerGauge)
	{
		UE_LOG(LogTemp, Log, TEXT("[Boss] 경직 게이지 MAX - 그로기 상태 진입"));
 
		if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
		{
			BossAI->SendStateTreeEvent(
				FGameplayTag::RequestGameplayTag(FName("Boss.Event.Stagger")));
		}
	}
}
 
bool ABossCharacterBase::IsStaggered_Implementation() const
{
	return CurrentCombatState == EBossCombatState::Stagger
		|| CurrentCombatState == EBossCombatState::BrainCrush;
}
 
// ============================================================
// ICombatState 구현
// ============================================================
 
bool ABossCharacterBase::IsAttacking_Implementation() const
{
	return CurrentCombatState == EBossCombatState::MeleeAttack
		|| CurrentCombatState == EBossCombatState::RangedAttack
		|| CurrentCombatState == EBossCombatState::Teleport;
}
 
bool ABossCharacterBase::HasSuperArmor_Implementation() const
{
	return bSuperArmor;
}
 
// ============================================================
// 보스 전용
// ============================================================
 
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
	MaxStaggerGauge = Config->MaxStaggerGauge;
	CurrentStaggerGauge = 0.f;
	CurrentPhase = EBossPhase::Phase1;
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 초기화 완료 - HP: %.0f, MaxStagger: %.0f"),
		MaxHPValue, MaxStaggerGauge);
}
 
// ============================================================
// 내부 메서드
// ============================================================
 
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
 
		switch (NewPhase)
		{
		case EBossPhase::Phase2:
			UE_LOG(LogTemp, Warning, TEXT("[Boss] Phase2 진입 - 맵 색상 변경 + ElectricOrbs 해금"));
			break;
		case EBossPhase::Phase2_Enhanced:
			UE_LOG(LogTemp, Warning, TEXT("[Boss] Phase2_Enhanced 진입 - TelekinesisThrow 해금"));
			break;
		case EBossPhase::Phase3_Cutscene:
			UE_LOG(LogTemp, Warning, TEXT("[Boss] Phase3 진입 - 컷씬 재생!"));
			break;
		default: break;
		}
 
		OnPhaseChanged.Broadcast(OldPhase, NewPhase);
 
		if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
		{
			BossAI->SendStateTreeEvent(
				FGameplayTag::RequestGameplayTag(FName("Boss.Event.PhaseTransition")));
		}
 
		CurrentStaggerGauge = 0.f;
		OnStaggerChanged.Broadcast(CurrentStaggerGauge, MaxStaggerGauge);
	}
}
 
void ABossCharacterBase::UpdateStaggerDecay(float DeltaTime)
{
	if (CurrentStaggerGauge <= 0.f) return;
	if (IsStaggered_Implementation()) return;
 
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastStaggerHitTime < StaggerDecayDelay) return;
 
	CurrentStaggerGauge = FMath::Max(0.f, CurrentStaggerGauge - StaggerDecayRate * DeltaTime);
	OnStaggerChanged.Broadcast(CurrentStaggerGauge, MaxStaggerGauge);
}
 
void ABossCharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[Boss] 사망 처리 시작"));
 
	CurrentCombatState = EBossCombatState::Death;
 
	if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
	{
		BossAI->SendStateTreeEvent(
			FGameplayTag::RequestGameplayTag(FName("Boss.Event.Death")));
	}
 
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

