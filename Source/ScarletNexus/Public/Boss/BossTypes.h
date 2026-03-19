// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BossTypes.generated.h"
 

// 보스 페이즈 정의
UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1_Probe		UMETA(DisplayName = "Phase 1: 탐색전"),
	Phase2_Assault		UMETA(DisplayName = "Phase 2: 본격 공세"),
	Phase3_Awakening	UMETA(DisplayName = "Phase 3: 각성"),
};
 
// 보스 전투 상태
UENUM(BlueprintType)
enum class EBossCombatState : uint8
{
	Idle,
	MeleeAttack,
	Teleport,
	RangedAttack,
	HitReaction,
	Stagger,
	BrainCrush,		// 스칼렛 넥서스 고유 메카닉
	Death,
};
 

// 공격 패턴 데이터

USTRUCT(BlueprintType)
struct FBossAttackPattern
{
	GENERATED_BODY()
 
	// 공격 식별 태그 (예: Boss.Attack.Melee.Combo3Hit)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AttackTag;
 
	// 사용할 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;
 
	// 기본 대미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 20.f;
 
	// 공격 범위 (근접/원거리 구분)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange = 200.f;
 
	// 쿨다운 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Cooldown = 2.f;
 
	// 이 패턴을 사용할 수 있는 최소 페이즈
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBossPhase MinPhase = EBossPhase::Phase1_Probe;
 
	// 패턴 선택 가중치 (높을수록 선택 확률 UP)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SelectionWeight = 1.f;
 
	// 텔레포트 연계 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanChainFromTeleport = false;
};
 
// ============================================================
// 페이즈 전환 설정
// ============================================================
USTRUCT(BlueprintType)
struct FBossPhaseConfig
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBossPhase Phase = EBossPhase::Phase1_Probe;
 
	// 이 페이즈로 전환되는 HP 비율 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HPThreshold = 1.f;
 
	// 페이즈 전환 시 재생할 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TransitionMontage = nullptr;
 
	// 이 페이즈에서 사용 가능한 공격 패턴 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FBossAttackPattern> AvailablePatterns;
 
	// 공격 간 기본 대기 시간 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D IdleDelayRange = FVector2D(1.0, 2.5);
 
	// 텔레포트 사용 빈도 가중치 (페이즈가 올라갈수록 증가)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TeleportFrequency = 0.3f;
};
 
// ============================================================
// 보스 전체 설정 Data Asset
// ============================================================
UCLASS(BlueprintType)
class SCARLETNEXUS_API UBossConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
 
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phases")
	TArray<FBossPhaseConfig> PhaseConfigs;
 
	// 최대 HP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxHP = 10000.f;
 
	// 경직(Stagger) 게이지 최대치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxStaggerGauge = 100.f;
 
	// 브레인 크래시 가능 경직 비율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BrainCrush")
	float BrainCrushThreshold = 0.8f;
 
	// 페이즈별 설정 조회 헬퍼
	const FBossPhaseConfig* GetPhaseConfig(EBossPhase Phase) const
	{
		return PhaseConfigs.FindByPredicate([Phase](const FBossPhaseConfig& Config)
		{
			return Config.Phase == Phase;
		});
	}
};
