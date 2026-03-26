// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PCTask_Chase.h"

#include "AIController.h"
#include "ScarletNexus.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"

EStateTreeRunStatus FPCTask_MoveToEnemy::EnterState(FStateTreeExecutionContext& Context,
                                                    const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	if (!data.Target) return EStateTreeRunStatus::Failed;
	
	AActor* owner = Cast<AActor>(Context.GetOwner());
	ACharacter* ownerChar = Cast<ACharacter>(owner);
	if (!ownerChar) return EStateTreeRunStatus::Failed;
	
	AAIController* AI = Cast<AAIController>(ownerChar->GetController());
	if (!AI) return EStateTreeRunStatus::Failed;
	
	AI->MoveToActor(data.Target, data.AcceptanceRadius);
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPCTask_MoveToEnemy::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner || !data.Target) return EStateTreeRunStatus::Failed;
	
	const float dist = FVector::Dist(owner->GetActorLocation(), data.Target->GetActorLocation());
#if WITH_EDITOR
	PRINTLOG_GT(TEXT("dist: %.1f, AcceptanceRadius: %.1f"), dist, data.AcceptanceRadius);
#endif
	
	// 공격 범위 도달 -> Succeeded로 Combat 전환
	if (dist <= data.AcceptanceRadius)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	// MoveToActor 갱신 (적이 움직이므로)
	ACharacter* ownerChar = Cast<ACharacter>(owner);
	if (AAIController* AI = Cast<AAIController>(ownerChar->GetController()))
	{
		AI->MoveToActor(data.Target, data.AcceptanceRadius);
	}
	
	return EStateTreeRunStatus::Running;
}

void FPCTask_MoveToEnemy::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (ACharacter* ownerChar = Cast<ACharacter>(owner))
	{
		if (AAIController* AI = Cast<AAIController>(ownerChar->GetController()))
		{
			AI->StopMovement();
		}
	}
}
