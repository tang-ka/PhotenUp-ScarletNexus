// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyTask_Die.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyBase.h"

static AEnemyBase* GetEnemyFromContext(FStateTreeExecutionContext& Context)
{
	if (AAIController* aic = Cast<AAIController>(Context.GetOwner()))
	{
		return Cast<AEnemyBase>(aic->GetPawn());
	}
	return Cast<AEnemyBase>(Context.GetOwner());
}

EStateTreeRunStatus FEnemyTask_Die::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	
	AEnemyBase* enemy = GetEnemyFromContext(Context);
	if (!enemy) return EStateTreeRunStatus::Failed;
	
	if (!data.bDieTriggered)
	{
		data.bDieTriggered = true;

		// 사망 몽타주 재생
		if (UEnemyAnimInstance* animInst = Cast<UEnemyAnimInstance>(enemy->GetMesh()->GetAnimInstance()))
		{
			animInst->PlayDieMontage();
		}
		
		enemy->Die();
	}
	
	return EStateTreeRunStatus::Succeeded;
}
