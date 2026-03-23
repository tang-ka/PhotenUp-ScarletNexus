// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PKTask_LiftObject.h"

#include "StateTreeExecutionContext.h"
#include "Interface/PKInteractable.h"
#include "PK/PKObject.h"

EStateTreeRunStatus FPKTask_LiftObject::EnterState(FStateTreeExecutionContext& Context,
                                                   const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	data.ElapsedTime = 0.f;
	if (!data.TargetPKObject) return EStateTreeRunStatus::Failed;
	
	IPKInteractable::Execute_OnPKPickuped(data.TargetPKObject);
	
	if (UPrimitiveComponent* prim = data.TargetPKObject->FindComponentByClass<UPrimitiveComponent>())
	{
		prim->SetSimulatePhysics(false);
		prim->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPKTask_LiftObject::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!data.TargetPKObject || !owner) return EStateTreeRunStatus::Failed;
	
	const FVector hoverTaget = owner->GetActorLocation() + owner->GetActorForwardVector() * 150.f + FVector(0, 0, data.LiftHeight);
	data.TargetPKObject->SetActorLocation(FMath::VInterpTo(data.TargetPKObject->GetActorLocation(), hoverTaget, DeltaTime, data.LiftInterpSpeed), true);
	data.ElapsedTime += DeltaTime;
	if (data.ElapsedTime >= data.AimReadyTime) return EStateTreeRunStatus::Succeeded;
	
	return EStateTreeRunStatus::Running;
}

void FPKTask_LiftObject::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	if (!data.TargetPKObject) return;
	
	IPKInteractable::Execute_OnPKReleased(data.TargetPKObject);
	
	if (UPrimitiveComponent* prim = data.TargetPKObject->FindComponentByClass<UPrimitiveComponent>())
	{
		prim->SetSimulatePhysics(true);
		prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
