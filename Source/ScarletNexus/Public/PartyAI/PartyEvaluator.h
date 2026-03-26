// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "PartyEvaluator.generated.h"

class APKObject;
class APlayerCharacterBase;

USTRUCT()
struct FPartyEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> OwnerActor = nullptr;
	
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
	
	// 적 상실 후 유지 시간
	UPROPERTY(EditAnywhere)
	float LoseEnemyDelay = 2.f;
	
	UPROPERTY()
	float TimeSinceEnemyLost = 0.f;
	
	// PK 오브젝트 탐색 추가
	UPROPERTY(EditAnywhere)
	float PKSearchRadius = 800.f;
	
	UPROPERTY(EditAnywhere, meta=(Output))
	TObjectPtr<APKObject> FoundPKObject = nullptr;
};

USTRUCT(DisplayName = "Party Evaluator")
struct FPartyEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPartyEvaluatorInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};