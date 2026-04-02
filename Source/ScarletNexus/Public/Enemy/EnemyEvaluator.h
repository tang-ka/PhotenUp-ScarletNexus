// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"

USTRUCT()
struct FEnemyEvaluatorInstanceData
{
	GENERATED_BODY()
	
	// 최근접 타겟 (플레이어 or 파티맴버)
	UPROPERTY(EditAnywhere, meta=(Output))
	TObjectPtr<AActor> NearestTarget = nullptr;
	
	// 탐지 반경
	UPROPERTY(EditAnywhere)
	float DetectRadius = 1500.f;
	
	// 공격 사거리
	UPROPERTY(EditAnywhere)
	float AttackRange = 200.f;
	
	// 타겟이 공격 범위 안에 있는지 체크
	UPROPERTY(EditAnywhere, meta=(Output))
	bool bInAttackRange = false;
	
	// 타겟까지 거리
	UPROPERTY(EditAnywhere, meta=(Output))
	float DistanceToTarget = 0.f;
};

USTRUCT(DisplayName="Enemy Evaluator")
struct FEnemyEvaluator : public FStateTreeEvaluatorBase
{
	GENERATED_BODY()
	using FInstanceDataType = FEnemyEvaluatorInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};