// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "EnemyTask_Stun.generated.h"

USTRUCT()
struct FEnemyTask_StunInstanceData
{
	GENERATED_BODY()
	
	// 스턴 시간
	UPROPERTY(EditAnywhere)
	float StunDuration = 0.3f;
	float ElapsedTime = 0.f;
};

USTRUCT(DisplayName="Enemy: Stun")
struct FEnemyTask_Stun : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FEnemyTask_StunInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};