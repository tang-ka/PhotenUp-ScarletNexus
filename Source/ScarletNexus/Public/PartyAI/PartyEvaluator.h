// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "PartyEvaluator.generated.h"

class APlayerCharacterBase;

USTRUCT()
struct FPartyEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta=(Output))
	TObjectPtr<APlayerCharacterBase> TrackedPlayer = nullptr;
	
	// 최근접 적 (IDamageable 적용체)
	UPROPERTY(EditAnywhere, meta=(Output))
	TObjectPtr<AActor> NearestEnemy = nullptr;
	
	UPROPERTY(EditAnywhere)
	float DetectRadius = 1200.f;
	
	UPROPERTY(EditAnywhere)
	float AttackRange = 500.f;
	
	UPROPERTY(EditAnywhere, meta=(Output))
	bool bInAttackRange = false;
};

USTRUCT(DisplayName = "Party Evaluator")
struct FPartyEvaluator : public FStateTreeEvaluatorBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPartyEvaluatorInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};