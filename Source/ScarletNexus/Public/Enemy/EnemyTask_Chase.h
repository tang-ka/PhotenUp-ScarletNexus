// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "EnemyTask_Chase.generated.h"

USTRUCT()
struct FEnemyTask_ChaseInstanceData
{
	GENERATED_BODY()
	
	// Evaluator에서 받아온 추적 대상
	UPROPERTY(EditAnywhere, meta = (Input))
	TObjectPtr<AActor> ChaseTarget = nullptr;
	
	// 도착 판정 거리
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 150.f;
	
	// MoveTo 갱신 주기 (초) : 갱신 딜레이
	UPROPERTY(EditAnywhere)
	float RepathInterval = 0.3f;
	float ElapsedSinceRepath = 0.f;
};

USTRUCT(DisplayName="Enemy: Chase Target")
struct FEnemyTask_Chase : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FEnemyTask_ChaseInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
