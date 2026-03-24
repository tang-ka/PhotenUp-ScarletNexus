// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossAttackExecutor.generated.h"
 
// 실행 중인 공격 타입
UENUM()
enum class EActiveAttackType : uint8
{
	None,
	TeleportKick,
	CloneRush,
};
 
// === 텔레포트 킥 내부 상태 ===
UENUM()
enum class ETKPhase : uint8
{
	Vanishing,
	Teleporting,
	Appearing,
	Kicking,
	Done,
};
 
// === 분신 돌진 내부 상태 ===
UENUM()
enum class ECRPhase : uint8
{
	// 분신 스폰 + 3체 나란히 준비
	SpawnClones,
	// 준비 동작 대기
	WindUp,
	// 왼쪽 분신 돌진
	LeftRush,
	// 왼쪽 돌진 후 잠깐 대기
	LeftDelay,
	// 오른쪽 분신 돌진
	RightRush,
	// 오른쪽 돌진 후 잠깐 대기
	RightDelay,
	// 본체 돌진
	BossRush,
	// 회복
	Recovery,
	Done,
};
 
USTRUCT()
struct FSTTask_BossAttackExecutorInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	// 선택된 공격 타입
	UPROPERTY()
	EActiveAttackType ActiveAttack = EActiveAttackType::None;
 
	UPROPERTY()
	float PhaseTimer = 0.f;
 
	UPROPERTY()
	bool bDamageApplied = false;
 
	// --- 텔레포트 킥 데이터 ---
	UPROPERTY()
	ETKPhase TKPhase = ETKPhase::Vanishing;
 
	UPROPERTY()
	FVector TKTarget = FVector::ZeroVector;
 
	// --- 분신 돌진 데이터 ---
	UPROPERTY()
	ECRPhase CRPhase = ECRPhase::SpawnClones;
 
	UPROPERTY()
	FVector CRDirection = FVector::ZeroVector;
 
	UPROPERTY()
	FVector CRStartLocation = FVector::ZeroVector;
 
	UPROPERTY()
	FVector CRTargetLocation = FVector::ZeroVector;
 
	// 스폰된 분신 액터 (왼쪽, 오른쪽)
	UPROPERTY()
	TObjectPtr<AActor> LeftClone = nullptr;
 
	UPROPERTY()
	TObjectPtr<AActor> RightClone = nullptr;
 
	// 본체 돌진용 데미지 플래그
	UPROPERTY()
	bool bBossRushDamageApplied = false;
};
 
USTRUCT(DisplayName = "Boss Attack Executor")
struct SCARLETNEXUS_API FSTTask_BossAttackExecutor : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossAttackExecutorInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;
 
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// --- 텔레포트 킥 설정 ---
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_VanishDuration = 0.2f;
 
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_AppearDelay = 0.1f;
 
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_KickDuration = 0.5f;
 
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_Damage = 150.f;
 
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_KickRadius = 200.f;
 
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_TeleportOffset = 250.f;
 
	UPROPERTY(EditAnywhere, Category = "TeleportKick")
	float TK_KnockbackForce = 1000.f;
 
	// --- 분신 돌진 설정 ---
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_WindUpDuration = 0.6f;
 
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_RushSpeed = 2500.f;
 
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_RushDistance = 1200.f;
 
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_RecoveryDuration = 0.5f;
 
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_Damage = 200.f;
 
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_RushWidth = 150.f;
 
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_KnockbackForce = 1200.f;
 
	// 분신 사이 간격 (보스 좌우 오프셋)
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_CloneSpacing = 200.f;
 
	// 각 분신 돌진 사이 딜레이
	UPROPERTY(EditAnywhere, Category = "CloneRush")
	float CR_SequenceDelay = 0.2f;
 
private:
	// 공격 선택
	EActiveAttackType SelectAttack() const;
 
	// 텔레포트 킥
	EStateTreeRunStatus EnterTeleportKick(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickTeleportKick(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	// 분신 돌진
	EStateTreeRunStatus EnterCloneRush(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickCloneRush(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	// 공통 데미지
	void ApplyDamageInRadius(AActor* BossActor, const FVector& Center, float Radius,
		float Damage, float Knockback, const FVector& KnockbackDir) const;
};