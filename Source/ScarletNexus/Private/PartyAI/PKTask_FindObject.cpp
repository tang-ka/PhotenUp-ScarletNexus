// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PKTask_FindObject.h"

#include "StateTreeExecutionContext.h"
#include "Engine/OverlapResult.h"
#include "Interface/PKInteractable.h"
#include "PK/PKObject.h"

EStateTreeRunStatus FPKTask_FindObject::EnterState(FStateTreeExecutionContext& Context,
                                                   const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner) return EStateTreeRunStatus::Failed;
	
	TArray<FOverlapResult> overlaps;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owner);
	
	owner->GetWorld()->OverlapMultiByChannel(
		overlaps,
		owner->GetActorLocation(),
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(data.SearchRadius),
		params);
	
	float bestDist = FLT_MAX;
	APKObject* best = nullptr;
	
	for (const FOverlapResult& hit : overlaps)
	{
		AActor* actor = hit.GetActor();
		if (!actor) continue;
		if (!actor->Implements<UPKInteractable>()) continue;
		if (!IPKInteractable::Execute_CanBePickeduped(actor)) continue;
		
		APKObject* pkActor = Cast<APKObject>(actor);
		if (!pkActor) continue;
		
		float dist = FVector::DistSquared(owner->GetActorLocation(), actor->GetActorLocation());
		if (dist < bestDist)
		{
			bestDist = dist;
			best = pkActor;
		}
	}
	
	if (!best) return EStateTreeRunStatus::Failed;
	data.FoundObject = best;
	return EStateTreeRunStatus::Succeeded;
}
