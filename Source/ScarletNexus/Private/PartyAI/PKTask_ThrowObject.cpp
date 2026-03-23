// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PKTask_ThrowObject.h"
#include "StateTreeExecutionContext.h"
#include "Interface/PKInteractable.h"
#include "PK/PKObject.h"
#include "Player/PlayerCharacterBase.h"

EStateTreeRunStatus FPKTask_ThrowObject::EnterState(FStateTreeExecutionContext& Context,
                                                    const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	
	if (!data.ThrownObject || !data.PartyTarget || !owner) return EStateTreeRunStatus::Failed;
	
	const FVector throwDir = ( data.PartyTarget->GetActorLocation() - data.ThrownObject->GetActorLocation() ).GetSafeNormal();
	
	IPKInteractable::Execute_OnPKThrown(data.ThrownObject, throwDir, data.ThrowSpeed);
	
	return EStateTreeRunStatus::Succeeded;
}
