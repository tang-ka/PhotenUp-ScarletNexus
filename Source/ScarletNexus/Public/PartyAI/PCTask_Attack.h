// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PCTask_Attack.generated.h"

class APlayerCharacterBase;

USTRUCT()
struct FPCTask_AttackInstanceData
{
	GENERATED_BODY()
	
	// Evaluator에서 받아온 타겟
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<AActor> Target = nullptr;
	
	// 공격 범위 
	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;
	
	// 데미지
	UPROPERTY(EditAnywhere)
	int32 Damage = 30;
	
	// 공격 쿨다운
	UPROPERTY(EditAnywhere)
	float AttackCooldown = 1.5f;

	// 재생할 몽타주 - ST 에디터에서 캐릭터별로 할당
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;
	
	// 내부 타이머
	float ElapsedTime = 0.f;
	bool bAttacked = false;
};

USTRUCT(DisplayName="Party: Melee Attack")
struct FPCTask_Attack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPCTask_AttackInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
