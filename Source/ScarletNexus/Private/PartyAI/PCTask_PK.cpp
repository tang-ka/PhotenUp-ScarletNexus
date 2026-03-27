#include "PartyAI/PCTask_PK.h"

#include "ScarletNexus.h"
#include "StateTreeExecutionContext.h"
#include "Engine/OverlapResult.h"
#include "Interface/PKInteractable.h"
#include "PK/PKObject.h"

EStateTreeRunStatus FPCTask_PK::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	data.FoundObject = nullptr;
	data.Phase = EPKPhase::Find;
	data.ElapsedTime = 0.f;

	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner) return EStateTreeRunStatus::Failed;

	// Find
	TArray<FOverlapResult> overlaps;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owner);

	owner->GetWorld()->OverlapMultiByChannel(
		overlaps, owner->GetActorLocation(), FQuat::Identity,
		ECC_WorldDynamic, FCollisionShape::MakeSphere(data.SearchRadius), params);

	float bestDist = FLT_MAX;
	APKObject* best = nullptr;

	for (const FOverlapResult& hit : overlaps)
	{
		AActor* actor = hit.GetActor();
		if (!actor) continue;
		if (!actor->Implements<UPKInteractable>()) continue;
		if (!data.FoundObject->Execute_CanBePickeduped(actor)) continue;

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

	// Lift 시작
	data.FoundObject->Execute_OnPKPickuped(data.FoundObject);
	data.Phase = EPKPhase::Lift;

#if WITH_EDITOR
	PRINTLOG_GT(TEXT("Phase: %s | FoundObject: %s"), *UEnum::GetValueAsString(data.Phase), *data.FoundObject.GetName())
#endif
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPCTask_PK::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner || !data.FoundObject)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (data.Phase == EPKPhase::Lift)
	{
		const FVector hoverTarget = owner->GetActorLocation() 
			+ owner->GetActorForwardVector() * 150.f 
			+ FVector(0, 0, data.LiftHeight);
		
		data.FoundObject->Execute_OnPKPickuped(data.FoundObject);
		// 타겟까지 끌고 가기
		data.FoundObject->SetActorLocation(FMath::VInterpTo(data.FoundObject->GetActorLocation(), hoverTarget, DeltaTime, data.LiftInterpSpeed), true);

		data.ElapsedTime += DeltaTime;
		if (data.ElapsedTime >= data.AimReadyTime)
		{
			data.Phase = EPKPhase::Throw;
		}
		return EStateTreeRunStatus::Running;
	}

	if (data.Phase == EPKPhase::Throw)
	{
#if WITH_EDITOR
		PRINTLOG_GT(TEXT("Phase: %s | TargetEnemy: %s"), *UEnum::GetValueAsString(data.Phase), data.TargetEnemy ? *data.TargetEnemy.GetName() : TEXT("Null"));
#endif
		
		if (!data.TargetEnemy) return EStateTreeRunStatus::Failed;

		// PK Object의 타입 분기
		switch (data.FoundObject->GetPKObjectType())
		{
		case EPKObjectType::Throwable:
			// 던지기
			const FVector throwDir = (data.TargetEnemy->GetActorLocation()
					- data.FoundObject->GetActorLocation()).GetSafeNormal();
			data.FoundObject->Execute_OnPKThrownPS(data.FoundObject, throwDir, data.ThrowSpeed);
			break;
		case EPKObjectType::Crumplable:
			break;
		case EPKObjectType::Rideable:
			break;
		}
		
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}

void FPCTask_PK::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	
#if WITH_EDITOR
	PRINTLOG_GT(TEXT("Phase: %s"), *UEnum::GetValueAsString(data.Phase));
#endif
	
	if (!data.FoundObject) return;
	
	data.FoundObject->Execute_OnPKReleased(data.FoundObject);
}