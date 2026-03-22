// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossSelectAttack.generated.h"
 

// Task Instance Data
USTRUCT()
struct FSTTask_BossSelectAttackInstanceData
{
	GENERATED_BODY()
 
	// --- 파라미터 (바인딩 선택사항) ---
 
	UPROPERTY(EditAnywhere, Category = "Parameter")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
 
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DistanceToPlayer = 0.f;
 
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bPlayerInSight = false;
 
	// --- 내부 상태 ---
 
	// 선택된 공격 패턴의 인덱스
	UPROPERTY()
	int32 SelectedPatternIndex = INDEX_NONE;
 
	// 현재 몽타주가 끝났는지
	UPROPERTY()
	bool bMontageFinished = false;
 
	// Context Actor
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
};
 

// Boss Select Attack Task

USTRUCT(DisplayName = "Boss select attack")
struct SCARLETNEXUS_API FSTTask_BossSelectAttack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossSelectAttackInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	// State 진입 시: 공격 패턴 선택 + 몽타주 재생 시작
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// Tick: 몽타주 완료 대기
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;
 
	// State 퇴장 시: 정리
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// --- 설정 (에디터에서 세팅) ---
 
	// 보스 설정 Data Asset 참조
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UBossConfigDataAsset> BossConfig = nullptr;
 
private:
	// 가중치 기반 랜덤 패턴 선택
	int32 SelectPatternByWeight(
		const TArray<FBossAttackPattern>& Patterns,
		float DistanceToPlayer,
		bool bPlayerInSight) const;
};


