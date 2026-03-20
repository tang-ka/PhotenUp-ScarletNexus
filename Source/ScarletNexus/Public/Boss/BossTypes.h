// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BossTypes.generated.h"
 


// 보스 페이즈 정의 (4단계, HP 기준 전환)
UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	// HP 100% ~ 70%: 기본 패턴 4종
	Phase1				UMETA(DisplayName = "Phase 1"),
 
	// HP 70% ~ 50%: 맵 색상 변경 + 전류구 추가
	Phase2				UMETA(DisplayName = "Phase 2"),
 
	// HP 50% ~ 25%: 염동력 사물 던지기 추가
	Phase2_Enhanced		UMETA(DisplayName = "Phase 2-2"),
 
	// HP 25% 도달: 컷씬 후 Phase2_Enhanced 패턴 반복
	Phase3_Cutscene		UMETA(DisplayName = "Phase 3"),
};
 

// 보스 전투 상태
UENUM(BlueprintType)
enum class EBossCombatState : uint8
{
	Idle,
	MeleeAttack,
	Teleport,
	RangedAttack,
	CloneAttack,
	HitReaction,
	Stagger,
	BrainCrush,
	PhaseTransition,
	Cutscene,
	Death,
};
 

// 공격 패턴 타입
UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	// Phase 1 기본 패턴
	TeleportKick,			// 순간이동 -> 옆차기 -> 뒤돌려차기
	CloneRush,				// 뒤 텔레포트 -> 3분신 순차 돌진
	AerialElectric,			// 위 텔레포트 -> 바닥 전류 방출
	IceSpikes,				// 바닥 얼음가시 생성
 
	// Phase 2 추가
	ElectricOrbs,			// 전류구 5개 -> 투척
 
	// Phase 2-2 추가
	TelekinesisThrow,		// 주변 사물 염동력으로 던지기
};
 

// 피격 리액션 종류
UENUM(BlueprintType)
enum class EHitReactionType : uint8
{
	Flinch,			// 약한 피격: 짧은 경직
	HeavyHit,		// 강한 피격: 행동 캔슬
	Knockback,		// 넉백: 밀려남
	Airborne,		// 에어본: 공중에 뜸
};
 

// 피격 방향
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right,
};
 

// 공격 패턴 데이터
USTRUCT(BlueprintType)
struct FBossAttackPattern
{
	GENERATED_BODY()
 
	// 공격 식별 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AttackTag;
 
	// 공격 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBossAttackType AttackType = EBossAttackType::TeleportKick;
 
	// 사용할 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;
 
	// 기본 대미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 20.f;
 
	// 공격 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange = 200.f;
 
	// 쿨다운 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Cooldown = 2.f;
 
	// 이 패턴이 처음 해금되는 페이즈
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBossPhase UnlockPhase = EBossPhase::Phase1;
 
	// 패턴 선택 가중치
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SelectionWeight = 1.f;
};
 

// 페이즈 전환 설정
USTRUCT(BlueprintType)
struct FBossPhaseConfig
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBossPhase Phase = EBossPhase::Phase1;
 
	// 이 페이즈로 전환되는 HP 비율 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HPThreshold = 1.f;
 
	// 페이즈 전환 시 재생할 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> TransitionMontage = nullptr;
 
	// 맵 색상 변경 여부 (Phase 2 진입 시)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bChangeMapColor = false;
 
	// 컷씬 재생 여부 (Phase 3 진입 시)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bPlayCutscene = false;
 
	// 공격 간 기본 대기 시간 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D IdleDelayRange = FVector2D(1.0, 2.5);
};
 

// 보스 전체 설정 Data Asset
UCLASS(BlueprintType)
class SCARLETNEXUS_API UBossConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
 
public:
	// 페이즈 설정 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phases")
	TArray<FBossPhaseConfig> PhaseConfigs;
 
	// 전체 공격 패턴 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attacks")
	TArray<FBossAttackPattern> AllAttackPatterns;
 
	// 최대 HP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxHP = 10000.f;
 
	// 경직 게이지 최대치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxStaggerGauge = 100.f;
 
	// 브레인 크래시 가능 경직 비율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|BrainCrush")
	float BrainCrushThreshold = 0.8f;
 
	// 페이즈별 설정 조회
	const FBossPhaseConfig* GetPhaseConfig(EBossPhase Phase) const
	{
		return PhaseConfigs.FindByPredicate([Phase](const FBossPhaseConfig& Config)
		{
			return Config.Phase == Phase;
		});
	}
 
	// 현재 페이즈에서 사용 가능한 패턴 조회 (이전 페이즈 패턴 포함)
	TArray<FBossAttackPattern> GetAvailablePatterns(EBossPhase CurrentPhase) const
	{
		TArray<FBossAttackPattern> Result;
		for (const FBossAttackPattern& Pattern : AllAttackPatterns)
		{
			if (Pattern.UnlockPhase <= CurrentPhase)
			{
				Result.Add(Pattern);
			}
		}
		return Result;
	}
};