// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "Engine/OverlapResult.h"
#include "Player/PlayerCharacterBase.h"

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
	
	APlayerCharacterBase* nearest = nullptr;
	float bestDist = FLT_MAX;
	
	for (const FOverlapResult& hit : overlaps)
	{
		APlayerCharacterBase* player = Cast<APlayerCharacterBase>(hit.GetActor());
		if (!player) continue;
		
		float dist = FVector::DistSquared(owner->GetActorLocation(), player->GetActorLocation());
		if (dist < bestDist)
		{
			bestDist = dist;
			nearest = player;
		}
	}
	
	data.TrackedPlayer = nearest;
	data.bInAttackRange = nearest ? (FVector::Dist(owner->GetActorLocation(), nearest->GetActorLocation()) <= data.AttackRange) : false;
}
