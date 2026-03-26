// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PCTask_Idle.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Player/PlayerCharacterBase.h"

EStateTreeRunStatus FPCTask_Follow::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	data.bIsMoving = false;
	
	if (!data.FollowTarget) return EStateTreeRunStatus::Failed;
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPCTask_Follow::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner || !data.FollowTarget) return EStateTreeRunStatus::Failed;
	
	ACharacter* ownerChar = Cast<ACharacter>(owner);
	if (!ownerChar) return EStateTreeRunStatus::Failed;
	
	AAIController* AI = Cast<AAIController>(ownerChar->GetController());
	if (!AI) return EStateTreeRunStatus::Failed;
	
	const float dist = FVector::Dist(owner->GetActorLocation(), data.FollowTarget->GetActorLocation());
	
	// 멀어지면 이동 시작
	if (!data.bIsMoving && dist > data.FollowStartDistance)
	{
		AI->MoveToActor(data.FollowTarget, data.AcceptanceRadius);
		data.bIsMoving = true;
	}
	// 가까워지면 정지
	else if (data.bIsMoving && dist <= data.AcceptanceRadius)
	{
		AI->StopMovement();
		data.bIsMoving = false;
	}
	
	return EStateTreeRunStatus::Running;
}

void FPCTask_Follow::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
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
