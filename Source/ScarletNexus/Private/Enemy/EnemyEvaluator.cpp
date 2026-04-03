// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "Engine/OverlapResult.h"
#include "PartyAI/PartyMemberBase.h"
#include "Player/PlayerCharacterBase.h"

void FEnemyEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorBase::TreeStart(Context);
}

void FEnemyEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner) return;
	
	const FVector ownerLocation = owner->GetActorLocation();
	
	TArray<FOverlapResult> overlaps;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owner);
	
	owner->GetWorld()->OverlapMultiByChannel(
		overlaps,
		ownerLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(data.DetectRadius),
		params);
	
	AActor* bestTarget = nullptr;
	float bestDist = FLT_MAX;
	
	for (const FOverlapResult& hit : overlaps)
	{
		AActor* actor = hit.GetActor();
		if (!actor) return;
		
		// 플레이어 캐릭터 or 파티 맴버를 타겟으로 인식
		bool bIsTarget = false;
		if (Cast<APlayerCharacterBase>(actor))
		{
			bIsTarget = true;
		}
		else if (APartyMemberBase* partyMember = Cast<APartyMemberBase>(actor))
		{
			bIsTarget = partyMember->IsAlive();
		}
		
		if (bestTarget)
		{
			const float dist = FVector::DistSquared(ownerLocation, actor->GetActorLocation());
			if (dist < bestDist)
			{
				bestDist = dist;
				bestTarget = actor;
			}
		}
	}
	
	data.NearestTarget = bestTarget;
	
	if (bestTarget)
	{
		data.DistanceToTarget = FVector::Dist(ownerLocation, bestTarget->GetActorLocation());
		data.bInAttackRange = data.DistanceToTarget <= data.AttackRange;
	}
	else
	{
		data.DistanceToTarget = 0.f;
		data.bInAttackRange = false;
	}
}
