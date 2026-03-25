// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "Engine/OverlapResult.h"
#include "Interface/Damageable.h"
#include "Interface/DamageableHelper.h"
#include "Player/PlayerCharacterBase.h"
#include "ProfilingDebugging/CookStats.h"

void FPartyEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner) return;
	
	TArray<FOverlapResult> overlaps;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owner);
	
	owner->GetWorld()->OverlapMultiByChannel(
		overlaps,
		owner->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(data.DetectRadius),
		params);
	
	// 파티
	APlayerCharacterBase* nearest = nullptr;
	float bestDist = FLT_MAX;
	
	// 적
	AActor* nearestEnemy = nullptr;
	float bestEnemyDist = FLT_MAX;
	
	for (const FOverlapResult& hit : overlaps)
	{
		AActor* actor = hit.GetActor();
		if (!actor) continue;
		
		// 파티 (플레이 캐릭터) 탐색
		if (APlayerCharacterBase* player = Cast<APlayerCharacterBase>(actor))
		{
			float dist = FVector::DistSquared(owner->GetActorLocation(), player->GetActorLocation());
			if (dist < bestDist)
			{
				bestDist = dist;
				nearest = player;
			}
			continue; // 플레이어 탐색하면 적 탐색 스킵
		}
		
		// 적 탐색
		if (DamageableHelpers::IsDamageable(actor))
		{
			float dist = FVector::DistSquared(owner->GetActorLocation(), actor->GetActorLocation());
			if (dist < bestEnemyDist)
			{
				bestEnemyDist = dist;
				nearestEnemy = actor;
			}
		}
	}
	
	
	data.TrackedPlayer = nearest;
	data.NearestEnemy = nearestEnemy;
	data.bInAttackRange = nearestEnemy ? (FVector::Dist(owner->GetActorLocation(), nearest->GetActorLocation()) <= data.AttackRange) : false;
}
