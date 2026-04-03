// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "EnemyTask_Die.generated.h"

USTRUCT()
struct FEnemyTask_DieInstanceData
{
	GENERATED_BODY()
	
	// Die 호출 여부 (중복 방지)
	bool bDieTriggered = false;
};

USTRUCT(DisplayName="Enemy: Die")
struct FEnemyTask_Die : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FEnemyTask_DieInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
