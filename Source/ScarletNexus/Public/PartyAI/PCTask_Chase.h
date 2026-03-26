// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PCTask_Chase.generated.h"

USTRUCT()
struct FPCTask_MoveToEnemyInstanceData
{
	GENERATED_BODY()
	
	// Evaluator에서 받아온 적
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<AActor> Target = nullptr;
	
	// 공격 사거리 안까지 접근
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 150.f;
};

USTRUCT(DisplayName="Party: Chase Enemy (Chase)")
struct FPCTask_MoveToEnemy : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPCTask_MoveToEnemyInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
