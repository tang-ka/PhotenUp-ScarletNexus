// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "BossTypes.h"
#include "STEval_BossPhase.generated.h"
 

// Evaluator Instance Data
// StateTree가 관리하는 런타임 데이터

USTRUCT()
struct FSTEval_BossPhaseInstanceData
{
	GENERATED_BODY()
 
	// 현재 페이즈
	UPROPERTY(EditAnywhere, Category = "Output")
	EBossPhase CurrentPhase = EBossPhase::Phase1_Probe;
 
	// 현재 HP 비율 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, Category = "Output")
	float HPRatio = 1.f;
 
	// 현재 경직 게이지 비율
	UPROPERTY(EditAnywhere, Category = "Output")
	float StaggerRatio = 0.f;
 
	// 플레이어와의 거리
	UPROPERTY(EditAnywhere, Category = "Output")
	float DistanceToPlayer = 0.f;
 
	// 플레이어가 시야 내에 있는지
	UPROPERTY(EditAnywhere, Category = "Output")
	bool bPlayerInSight = false;
 
	// 브레인 크래시 가능 상태인지
	UPROPERTY(EditAnywhere, Category = "Output")
	bool bBrainCrushAvailable = false;
 
	// 페이즈 전환 중인지
	UPROPERTY(EditAnywhere, Category = "Output")
	bool bIsPhaseTransitioning = false;
 
	// 마지막 공격 이후 경과 시간
	UPROPERTY(EditAnywhere, Category = "Output")
	float TimeSinceLastAttack = 0.f;
 
	// Context Actor (보스 캐릭터) - StateTree에서 자동 바인딩
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
};
 

// Boss Phase Evaluator
USTRUCT(DisplayName = "Boss phase evaluator")
struct SCARLETNEXUS_API FSTEval_BossPhase : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTEval_BossPhaseInstanceData;
 
	// InstanceData 타입 등록
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	// StateTree 시작 시 초기화
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
 
	// 매 Tick마다 실행 - 월드 상태를 관찰하고 InstanceData 갱신
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
 
private:
	// HP 비율에 따른 페이즈 결정
	EBossPhase DeterminePhase(float HPRatio) const;
};
