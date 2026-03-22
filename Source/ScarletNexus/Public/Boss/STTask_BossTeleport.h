// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossTeleport.generated.h"
 
UENUM(BlueprintType)
enum class ETeleportStrategy : uint8
{
	// 플레이어 뒤쪽으로 이동
	BehindPlayer,
	// 플레이어 측면으로 이동
	FlankPlayer,
	// 랜덤 위치로 이동 (Phase 3 전용: 잔상 효과)
	RandomWithAfterimage,
	// 공중 텔레포트 (내려찍기 연계)
	AerialAbove,
};
 
USTRUCT()
struct FSTTask_BossTeleportInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	float DistanceToPlayer = 0.f;
 
	UPROPERTY(EditAnywhere, Category = "Input")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
 
	// 텔레포트 목표 위치 (내부 계산)
	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;
 
	// 텔레포트 진행 시간
	UPROPERTY()
	float ElapsedTime = 0.f;
 
	// 텔레포트 완료 여부
	UPROPERTY()
	bool bTeleportComplete = false;
 
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
};
 
USTRUCT(DisplayName = "Boss teleport")
struct SCARLETNEXUS_API FSTTask_BossTeleport : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossTeleportInstanceData;
 
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
 
	// --- 설정 ---
 
	// 텔레포트 전략
	UPROPERTY(EditAnywhere, Category = "Config")
	ETeleportStrategy Strategy = ETeleportStrategy::BehindPlayer;
 
	// 텔레포트 소요 시간 (사라짐 -> 등장 연출 시간)
	UPROPERTY(EditAnywhere, Category = "Config")
	float TeleportDuration = 0.3f;
 
	// 목표 지점에서 플레이어까지의 오프셋 거리
	UPROPERTY(EditAnywhere, Category = "Config")
	float OffsetDistance = 200.f;
 
	// 텔레포트 시 이펙트 (Niagara System)
	// UPROPERTY(EditAnywhere, Category = "Config")
	// TObjectPtr<UNiagaraSystem> TeleportVFX = nullptr;
 
private:
	FVector CalculateTargetLocation(
		const AActor* BossActor,
		const AActor* PlayerActor,
		ETeleportStrategy InStrategy) const;
};
