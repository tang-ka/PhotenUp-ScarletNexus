// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_CloneRush.generated.h"
 
// 돌진 진행 단계
UENUM()
enum class ECloneRushPhase : uint8
{
	// 돌진 준비 (잠깐 멈추고 자세 잡기)
	WindUp,
	// 돌진 중 (고속 이동 + 경로 데미지)
	Rushing,
	// 돌진 후 감속 & 복귀
	Recovery,
	// 완료
	Done,
};
 
USTRUCT()
struct FSTTask_CloneRushInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	UPROPERTY()
	ECloneRushPhase AttackPhase = ECloneRushPhase::WindUp;
 
	UPROPERTY()
	float PhaseTimer = 0.f;
 
	// 돌진 방향 (EnterState에서 고정)
	UPROPERTY()
	FVector RushDirection = FVector::ZeroVector;
 
	// 돌진 시작 위치
	UPROPERTY()
	FVector StartLocation = FVector::ZeroVector;
 
	// 돌진 목표 위치 (플레이어 관통 후 뒤쪽)
	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;
 
	// 데미지 적용 여부
	UPROPERTY()
	bool bDamageApplied = false;
};
 
USTRUCT(DisplayName = "Clone Rush Attack")
struct SCARLETNEXUS_API FSTTask_CloneRush : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_CloneRushInstanceData;
 
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
 
	// 준비 시간 (초) - 플레이어를 향해 회전하고 잠깐 대기
	UPROPERTY(EditAnywhere, Category = "Config")
	float WindUpDuration = 0.4f;
 
	// 돌진 속도 (cm/s)
	UPROPERTY(EditAnywhere, Category = "Config")
	float RushSpeed = 2500.f;
 
	// 돌진 거리 (플레이어 관통 후 추가 거리 포함)
	UPROPERTY(EditAnywhere, Category = "Config")
	float RushDistance = 800.f;
 
	// 돌진 후 회복 시간
	UPROPERTY(EditAnywhere, Category = "Config")
	float RecoveryDuration = 0.5f;
 
	// 돌진 데미지
	UPROPERTY(EditAnywhere, Category = "Config")
	float RushDamage = 200.f;
 
	// 돌진 판정 폭 (반경)
	UPROPERTY(EditAnywhere, Category = "Config")
	float RushWidth = 150.f;
 
	// 넉백 힘
	UPROPERTY(EditAnywhere, Category = "Config")
	float KnockbackForce = 1200.f;
 
private:
	void ApplyRushDamage(AActor* BossActor, const FVector& RushDir) const;
};
