// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyTask_Die.h"

#include "StateTreeExecutionContext.h"
#include "Enemy/EnemyBase.h"

EStateTreeRunStatus FEnemyTask_Die::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	
	AEnemyBase* enemy = Cast<AEnemyBase>(Context.GetOwner());
	if (!enemy) return EStateTreeRunStatus::Failed;
	
	if (!data.bDieTriggered)
	{
		data.bDieTriggered = true;
		enemy->Die();
	}
	
	return EStateTreeRunStatus::Succeeded;
}
