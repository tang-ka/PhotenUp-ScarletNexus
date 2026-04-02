// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "EnemyTask_Attack.generated.h"

USTRUCT()
struct FEnemyTask_AttackInstanceData
{
	GENERATED_BODY()
	
	// Evaluator에서 받아온 타겟
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<AActor> Target = nullptr;
	
	// 공격 범위
	UPROPERTY(EditAnywhere)
	float AttackRadius = 200.f;
	
	// 공격 데미지 (적 -> 플레이어)
	UPROPERTY(EditAnywhere)
	int32 AtkDmg = 25;
	
	// 공격 쿨다운
	UPROPERTY(EditAnywhere)
	float AttackCooldown = 1.8f;
	
	// 쿨다운 중 데미지 적용 시점 비율 ( 0 ~ 1 )
	UPROPERTY(EditAnywhere)
	float DamageTimingRatio = 0.35f;
	
	// 내부 
	float ElapsedTime = 0.0f;
	bool bDamageApplied = false;
};

USTRUCT(DisplayName="Enemy: Attack")
struct FEnemyTask_Attack : public FStateTreeTaskBase
{
	GENERATED_BODY()
	using FInstanceDataType = FEnemyTask_AttackInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
