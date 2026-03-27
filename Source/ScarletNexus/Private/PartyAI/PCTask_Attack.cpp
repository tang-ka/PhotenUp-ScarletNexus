// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PCTask_Attack.h"

#include "ScarletNexus.h"
#include "StateTreeExecutionContext.h"
#include "Boss/BossCharacterBase.h"
#include "Interface/Damageable.h"
#include "Interface/DamageableHelper.h"
#include "PartyAI/PartyMemberBase.h"

EStateTreeRunStatus FPCTask_Attack::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	
#if WITH_EDITOR
	PRINTLOG_GT(TEXT("Attack Target: %s"), data.Target ? *data.Target->GetName() : TEXT("NULL"));
#endif
	
	data.ElapsedTime = 0.f;
	data.bAttacked = false;
	
	// IDamageable 적용 체크
	if (!data.Target || !DamageableHelpers::IsDamageable(data.Target)/*Cast<ABossCharacterBase>(data.Target)*/) return EStateTreeRunStatus::Failed;
	
	// 타겟 방향으로 회전
	AActor* owner = Cast<AActor>(data.Target);
	if (owner)
	{
		FVector dir = (data.Target->GetActorLocation() - owner->GetActorLocation()).GetSafeNormal2D();
		if (!dir.IsNearlyZero())
		{
			owner->SetActorRotation(dir.Rotation());
		}
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPCTask_Attack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner || !data.Target) return EStateTreeRunStatus::Failed;
	
	data.ElapsedTime += DeltaTime;
	
	// 쿨다운 중간 시점에 한 번 공격
	if (!data.bAttacked && data.ElapsedTime >= data.AttackCooldown * 0.3f)
	{
		const float dist = FVector::Dist(owner->GetActorLocation(), data.Target->GetActorLocation());
		APartyMemberBase* partyMember = Cast<APartyMemberBase>(owner);
		if (!partyMember) return EStateTreeRunStatus::Failed;
		if (dist <= data.AttackRadius)
		{
			FDamageInfo info;
			info.DamageAmount = data.Damage;
			info.DamageCauser = owner;
			partyMember.
			IDamageable::Execute_ReceiveDamage(data.Target, info);
		}
		data.bAttacked = true;
	}
	
	// 쿨다운 완료 -> Succeeded로 Combat 루프 재진입
	if (data.ElapsedTime >= data.AttackCooldown) return EStateTreeRunStatus::Succeeded;
	return EStateTreeRunStatus::Running;
}
