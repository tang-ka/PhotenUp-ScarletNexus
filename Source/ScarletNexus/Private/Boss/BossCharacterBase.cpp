// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossCharacterBase.h"
#include "Boss/BossAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
 
ABossCharacterBase::ABossCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
 
	// AI Controller 클래스 지정
	AIControllerClass = ABossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
 
	// 캐릭터 무브먼트 기본 설정
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 500.f;
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
	}
 
	// 컨트롤러 회전 사용하지 않음 (이동 방향으로 자동 회전)
	bUseControllerRotationYaw = false;
}
 
void ABossCharacterBase::BeginPlay()
{
	Super::BeginPlay();
 
	// Config가 에디터에서 설정되어 있으면 초기화
	if (BossConfig)
	{
		InitializeWithConfig_Implementation(BossConfig);
	}
}
 
void ABossCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
	// 경직 게이지 자연 감소
	UpdateStaggerDecay(DeltaTime);
}
 
// ============================================================
// IBossCharacterInterface 구현
// ============================================================
 
float ABossCharacterBase::GetHPRatio_Implementation() const
{
	if (MaxHP <= 0.f)
	{
		return 0.f;
	}
	return FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f);
}
 
float ABossCharacterBase::GetCurrentHP_Implementation() const
{
	return CurrentHP;
}
 
void ABossCharacterBase::ApplyDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (CurrentHP <= 0.f)
	{
		return; // 이미 사망
	}
 
	const float OldHP = CurrentHP;
	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.f, MaxHP);
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 대미지: %.0f (HP: %.0f -> %.0f / %.0f)"),
		DamageAmount, OldHP, CurrentHP, MaxHP);
 
	// HP 변경 이벤트 브로드캐스트
	OnHPChanged.Broadcast(CurrentHP, MaxHP, DamageAmount);
 
	// 페이즈 전환 체크
	CheckPhaseTransition();
 
	// 사망 체크
	if (CurrentHP <= 0.f)
	{
		HandleDeath();
	}
}
 
float ABossCharacterBase::GetStaggerRatio_Implementation() const
{
	if (MaxStaggerGauge <= 0.f)
	{
		return 0.f;
	}
	return FMath::Clamp(CurrentStaggerGauge / MaxStaggerGauge, 0.f, 1.f);
}
 
void ABossCharacterBase::ApplyStaggerDamage_Implementation(float StaggerAmount)
{
	// 슈퍼아머 상태면 경직 대미지 무시
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
 
	// 경직 게이지 MAX 도달 시 -> AI Controller에 이벤트 전달
	if (CurrentStaggerGauge >= MaxStaggerGauge)
	{
		UE_LOG(LogTemp, Log, TEXT("[Boss] 경직 게이지 MAX - 그로기 상태 진입"));
 
		if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
		{
			// Boss.Event.Stagger 태그로 StateTree에 이벤트 전달
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
 
EBossCombatState ABossCharacterBase::GetCombatState_Implementation() const
{
	return CurrentCombatState;
}
 
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
 
void ABossCharacterBase::InitializeWithConfig_Implementation(UBossConfigDataAsset* Config)
{
	if (!Config)
	{
		UE_LOG(LogTemp, Error, TEXT("[Boss] InitializeWithConfig: Config가 nullptr입니다."));
		return;
	}
 
	BossConfig = Config;
	MaxHP = Config->MaxHP;
	CurrentHP = MaxHP;
	MaxStaggerGauge = Config->MaxStaggerGauge;
	CurrentStaggerGauge = 0.f;
	CurrentPhase = EBossPhase::Phase1_Probe;
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 초기화 완료 - HP: %.0f, MaxStagger: %.0f"),
		MaxHP, MaxStaggerGauge);
}
 
// 내부 메서드
void ABossCharacterBase::CheckPhaseTransition()
{
	if (!BossConfig)
	{
		return;
	}
 
	const float HPRatio = GetHPRatio_Implementation();
	EBossPhase NewPhase = CurrentPhase;
 
	// HP 비율에 따른 페이즈 결정
	// PhaseConfigs는 Phase1(HP 1.0~0.7), Phase2(0.7~0.35), Phase3(0.35~0)
	if (HPRatio <= 0.35f)
	{
		NewPhase = EBossPhase::Phase3_Awakening;
	}
	else if (HPRatio <= 0.70f)
	{
		NewPhase = EBossPhase::Phase2_Assault;
	}
 
	if (NewPhase != CurrentPhase)
	{
		const EBossPhase OldPhase = CurrentPhase;
		CurrentPhase = NewPhase;
 
		UE_LOG(LogTemp, Log, TEXT("[Boss] 페이즈 전환: %d -> %d (HP Ratio: %.2f)"),
			static_cast<int32>(OldPhase),
			static_cast<int32>(NewPhase),
			HPRatio);
 
		// 페이즈 전환 이벤트 브로드캐스트
		OnPhaseChanged.Broadcast(OldPhase, NewPhase);
 
		// AI Controller에 페이즈 전환 이벤트 전달
		if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
		{
			BossAI->SendStateTreeEvent(
				FGameplayTag::RequestGameplayTag(FName("Boss.Event.PhaseTransition")));
		}
 
		// 페이즈 전환 시 경직 게이지 리셋
		CurrentStaggerGauge = 0.f;
		OnStaggerChanged.Broadcast(CurrentStaggerGauge, MaxStaggerGauge);
	}
}
 
void ABossCharacterBase::UpdateStaggerDecay(float DeltaTime)
{
	if (CurrentStaggerGauge <= 0.f)
	{
		return;
	}
 
	// 경직 상태에서는 자연 감소하지 않음
	if (IsStaggered_Implementation())
	{
		return;
	}
 
	// 마지막 피격 후 일정 시간이 지나야 감소 시작
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastStaggerHitTime < StaggerDecayDelay)
	{
		return;
	}
 
	CurrentStaggerGauge = FMath::Max(0.f, CurrentStaggerGauge - StaggerDecayRate * DeltaTime);
	OnStaggerChanged.Broadcast(CurrentStaggerGauge, MaxStaggerGauge);
}
 
void ABossCharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[Boss] 사망 처리 시작"));
 
	CurrentCombatState = EBossCombatState::Death;
 
	// AI Controller에 사망 이벤트 전달
	if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
	{
		BossAI->SendStateTreeEvent(
			FGameplayTag::RequestGameplayTag(FName("Boss.Event.Death")));
	}
 
	// 무브먼트 비활성화
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
 
	// 콜리전 비활성화 (통과 가능)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 
	// TODO: 사망 몽타주 재생
	// TODO: 사망 이펙트 재생
	// TODO: 일정 시간 후 액터 제거 또는 비가시화
}

