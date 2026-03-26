// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyEvaluator.h"

#include "ScarletNexus.h"
#include "StateTreeExecutionContext.h"
#include "Boss/BossCharacterBase.h"
#include "Engine/OverlapResult.h"
#include "Interface/Damageable.h"
#include "Interface/DamageableHelper.h"
#include "Player/PlayerCharacterBase.h"
#include "ProfilingDebugging/CookStats.h"

void FPartyEvaluator::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorBase::TreeStart(Context);
	PRINTLOG_GT(TEXT("TreeStart"));
}

void FPartyEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	//PRINTLOG_GT(TEXT("Tick 호출"));
	auto& data = Context.GetInstanceData(*this);
	
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner) return;
	
	// 디버깅용
	data.OwnerActor = owner;
	
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
				//RINTLOG_GT(TEXT("최근접 플레이어: %s"), *nearest->GetName());
			}
			continue; // 플레이어 탐색하면 적 탐색 스킵
		}
		
		// 적 탐색
		if (Cast<ABossCharacterBase>(actor))
		//if (DamageableHelpers::IsDamageable(actor))
		{
			float dist = FVector::DistSquared(owner->GetActorLocation(), actor->GetActorLocation());
			if (dist < bestEnemyDist)
			{
				bestEnemyDist = dist;
				nearestEnemy = actor;
				//PRINTLOG_GT(TEXT("최근접 적: %s"), *nearestEnemy->GetName());
			}
		}
	}
	
	// 적 찾으면 갱신
	if (nearestEnemy)
	{
		data.NearestEnemy = nearestEnemy;
		data.TimeSinceEnemyLost = 0.f;
	}
	else if (data.NearestEnemy)
	{
		// 이전에 적이 있었는데 이번 틱에는 못 찾은 경우 -> 딜레이 후 해제
		data.TimeSinceEnemyLost += DeltaTime;
		if (data.TimeSinceEnemyLost >= data.LoseEnemyDelay)
		{
			data.NearestEnemy = nullptr;
		}
	}
	
	data.TrackedPlayer = nearest;
	data.bInAttackRange = nearestEnemy ? (FVector::Dist(owner->GetActorLocation(), nearestEnemy->GetActorLocation()) <= data.AttackRange) : false;
	
#if WITH_EDITOR
	//PRINTLOG_GT(TEXT("NearestEnemy: %s, bInAttackRange: %d"), nearestEnemy ? *nearestEnemy->GetName() : TEXT("Null"), data.bInAttackRange);
#endif
}
