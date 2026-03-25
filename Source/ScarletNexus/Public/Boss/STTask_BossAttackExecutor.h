// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossAttackExecutor.generated.h"
 
class ABossCharacterBase;
 
UENUM()
enum class EActiveAttackType : uint8
{
	None,
	TeleportKick,      // Phase1
	CloneRush,         // Phase1
	AerialElectric,    // Phase1
	IceSpikes,         // Phase1
	ElectricOrbs,      // Phase2+
	TelekinesisThrow,  // Phase2_Enhanced+
};
 
// === 내부 상태 Enums ===
UENUM()
enum class ETKPhase : uint8 { Vanishing, Teleporting, Appearing, Kicking, Done };
 
UENUM()
enum class ECRPhase : uint8 { SpawnClones, WindUp, LeftRush, LeftDelay, RightRush, RightDelay, BossRush, Recovery, Done };
 
UENUM()
enum class EAEPhase : uint8 { Vanishing, Hovering, Charging, Discharge, Landing, Done };
 
UENUM()
enum class EISPhase : uint8 { WindUp, Warning, Spawning, Holding, Fading, Done };
 
UENUM()
enum class EOOPhase : uint8
{
	Charging,    // 보스 주변에 전류구 생성
	Flying,      // 전부 발사 -> 플레이어 추적 비행
	Done,
};
 
UENUM()
enum class ETTPhase : uint8
{
	Gathering,   // 주변 사물 염동력으로 들어올림
	Aiming,      // 플레이어 조준
	Throwing,    // 투척
	Done,
};
 
USTRUCT()
struct FSTTask_BossAttackExecutorInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	UPROPERTY()
	EActiveAttackType ActiveAttack = EActiveAttackType::None;
 
	UPROPERTY()
	float PhaseTimer = 0.f;
 
	UPROPERTY()
	bool bDamageApplied = false;
 
	// 텔레포트 킥 
	UPROPERTY()
	ETKPhase TKPhase = ETKPhase::Vanishing;
	UPROPERTY()
	FVector TKTarget = FVector::ZeroVector;
 
	// 분신 돌진 
	UPROPERTY()
	ECRPhase CRPhase = ECRPhase::SpawnClones;
	UPROPERTY()
	FVector CRDirection = FVector::ZeroVector;
	UPROPERTY()
	FVector CRStartLocation = FVector::ZeroVector;
	UPROPERTY()
	FVector CRTargetLocation = FVector::ZeroVector;
	UPROPERTY()
	TObjectPtr<AActor> LeftClone = nullptr;
	UPROPERTY()
	TObjectPtr<AActor> RightClone = nullptr;
	UPROPERTY()
	bool bBossRushDamageApplied = false;
 
	// 공중 전류
	UPROPERTY()
	EAEPhase AEPhase = EAEPhase::Vanishing;
	UPROPERTY()
	FVector AEHoverLocation = FVector::ZeroVector;
	UPROPERTY()
	FVector AEGroundTarget = FVector::ZeroVector;
	UPROPERTY()
	FVector AEOriginalLocation = FVector::ZeroVector;
 
	// 얼음가시
	UPROPERTY()
	EISPhase ISPhase = EISPhase::WindUp;
	UPROPERTY()
	FVector ISDirection = FVector::ZeroVector;
	UPROPERTY()
	FVector ISOrigin = FVector::ZeroVector;
	UPROPERTY()
	int32 ISSpawnedCount = 0;
	UPROPERTY()
	float ISSpawnTimer = 0.f;
 
	// 전류구
	UPROPERTY()
	EOOPhase OOPhase = EOOPhase::Charging;
	UPROPERTY()
	int32 OOLaunchedCount = 0;
	UPROPERTY()
	float OOLaunchTimer = 0.f;
	// 각 전류구 위치 (발사 전 보스 주변, 발사 후 이동 중)
	UPROPERTY()
	TArray<FVector> OOOrbPositions;
	UPROPERTY()
	TArray<FVector> OOOrbDirections;
	UPROPERTY()
	TArray<bool> OOOrbHit;
 
	// --- 염동력 투척 ---
	UPROPERTY()
	ETTPhase TTPhase = ETTPhase::Gathering;
	UPROPERTY()
	TArray<FVector> TTDebrisPositions;
	UPROPERTY()
	FVector TTTargetLocation = FVector::ZeroVector;
	UPROPERTY()
	int32 TTThrownCount = 0;
	UPROPERTY()
	float TTThrowTimer = 0.f;
};
 
USTRUCT(DisplayName = "Boss Attack Executor")
struct SCARLETNEXUS_API FSTTask_BossAttackExecutor : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossAttackExecutorInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
 
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
 
	// 텔레포트 킥
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_VanishDuration = 0.2f;
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_AppearDelay = 0.1f;
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_KickDuration = 0.5f;
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_Damage = 150.f;
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_KickRadius = 200.f;
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_TeleportOffset = 250.f;
	UPROPERTY(EditAnywhere, Category = "TeleportKick") float TK_KnockbackForce = 1000.f;
 
	// 분신 돌진 
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_WindUpDuration = 0.6f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_RushSpeed = 3500.f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_RushDistance = 1200.f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_RecoveryDuration = 0.5f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_Damage = 200.f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_RushWidth = 150.f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_KnockbackForce = 1200.f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_CloneSpacing = 200.f;
	UPROPERTY(EditAnywhere, Category = "CloneRush") float CR_SequenceDelay = 0.2f;
 
	// 공중 전류
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_VanishDuration = 0.2f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_HoverHeight = 300.f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_ChargeDuration = 0.8f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_DischargeDuration = 0.6f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_DamageRadius = 400.f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_Damage = 180.f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_LandingDuration = 0.3f;
	UPROPERTY(EditAnywhere, Category = "AerialElectric") float AE_KnockbackForce = 800.f;
 
	// 얼음가시
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_WindUpDuration = 0.5f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_WarningDuration = 0.8f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") int32 IS_SpikeCount = 50;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_AreaLength = 1200.f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_AreaWidth = 1000.f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_AreaStartOffset = 150.f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_HoldDuration = 1.0f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_Damage = 120.f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_SpikeRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = "IceSpikes") float IS_KnockbackForce = 600.f;
 
	// 전류구 (Phase2부터)
	// 보스 주변에 전류구 생성 후 순차 발사
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") int32 OO_OrbCount = 5;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_ChargeDuration = 0.6f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_LaunchInterval = 0.15f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_OrbSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_OrbMaxDistance = 2000.f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_Damage = 100.f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_OrbRadius = 20.f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_KnockbackForce = 500.f;
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_OrbitRadius = 150.f;
	// 추적 회전 속도 (초당 방향 보간 비율, 1.0 = 즉시 추적, 0.5 = 느리게)
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_TrackingStrength = 90.0f;
	// 전류구 시간 최대 5초
	UPROPERTY(EditAnywhere, Category = "ElectricOrbs") float OO_MaxLifetime = 4.f;
 
	// // 염동력 투척 (Phase2_Enhanced+)
	// // 주변 사물을 들어올려 플레이어에게 투척
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") int32 TT_DebrisCount = 3;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_GatherDuration = 0.8f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_AimDuration = 0.3f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_ThrowInterval = 0.2f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_ThrowSpeed = 3000.f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_MaxDistance = 2000.f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_Damage = 160.f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_HitRadius = 120.f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_KnockbackForce = 900.f;
	// UPROPERTY(EditAnywhere, Category = "TelekinesisThrow") float TT_LiftHeight = 300.f;
 
private:
	EActiveAttackType SelectAttack(const ACharacter* Boss) const;
 
	EStateTreeRunStatus EnterTeleportKick(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickTeleportKick(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	EStateTreeRunStatus EnterCloneRush(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickCloneRush(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	EStateTreeRunStatus EnterAerialElectric(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickAerialElectric(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	EStateTreeRunStatus EnterIceSpikes(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickIceSpikes(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	EStateTreeRunStatus EnterElectricOrbs(FInstanceDataType& Data, ACharacter* Boss) const;
	EStateTreeRunStatus TickElectricOrbs(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	// EStateTreeRunStatus EnterTelekinesisThrow(FInstanceDataType& Data, ACharacter* Boss) const;
	// EStateTreeRunStatus TickTelekinesisThrow(FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const;
 
	void ApplyDamageInRadius(AActor* BossActor, const FVector& Center, float Radius,
		float Damage, float Knockback, const FVector& KnockbackDir) const;
};
