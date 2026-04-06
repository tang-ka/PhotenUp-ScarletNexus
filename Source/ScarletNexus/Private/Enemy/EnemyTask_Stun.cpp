// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyTask_Stun.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyBase.h"

// Owner(Controller) -> EnemyBase 헬퍼
static AEnemyBase* GetEnemyFromContext_Stun(FStateTreeExecutionContext& Context)
{
	if (AAIController* aic = Cast<AAIController>(Context.GetOwner()))
	{
		return Cast<AEnemyBase>(aic->GetPawn());
	}
	return Cast<AEnemyBase>(Context.GetOwner());
}

EStateTreeRunStatus FEnemyTask_Stun::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition) const
{
	auto data = Context.GetInstanceData(*this);
	data.ElapsedTime = 0.f;
	
	AEnemyBase* enemy = GetEnemyFromContext_Stun(Context);
	if (!enemy) return EStateTreeRunStatus::Failed;
	
	// 이동 정지
	if (AAIController* aic = Cast<AAIController>(enemy->GetController()))
	{
		aic->StopMovement();
	}
	
	// 히트 몽타주 재생
	if (enemy->HitReactionMontage)
	{
		if (UEnemyAnimInstance* animInst = Cast<UEnemyAnimInstance>(enemy->GetMesh()->GetAnimInstance()))
		{
			animInst->PlayHitReactMontage();
		}
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FEnemyTask_Stun::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto data = Context.GetInstanceData(*this);
	data.ElapsedTime += DeltaTime;
	
	// 스턴 시간 지나면 Succeeded -> 이전 행동으로 복귀
	if (data.ElapsedTime >= data.StunDuration)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}

void FEnemyTask_Stun::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AEnemyBase* enemy = GetEnemyFromContext_Stun(Context);
	if (!enemy) return;
	
	// 스턴 해제
	enemy->bIsStunned = false;
}
