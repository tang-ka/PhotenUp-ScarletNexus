// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/AttackTask_MeleeAttack.h"

#include "ScarletNexus.h"
#include "StateTreeExecutionContext.h"
#include "Boss/BossCharacterBase.h"
#include "Interface/Damageable.h"
#include "Interface/DamageableHelper.h"

EStateTreeRunStatus FAttackTask_MeleeAttack::EnterState(FStateTreeExecutionContext& Context,
                                                        const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	
#if WITH_EDITOR
	PRINTLOG_GT(TEXT("Attack Target: %s"), data.Target ? *data.Target->GetName() : TEXT("NULL"));
#endif
	
	data.ElapsedTime = 0.f;
	data.bAttacked = false;
	
	// IDamageable 적용 체크
	if (/*!DamageableHelpers::IsDamageable(data.Target)*/Cast<ABossCharacterBase>(data.Target)) return EStateTreeRunStatus::Failed;
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAttackTask_MeleeAttack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner || !data.Target) return EStateTreeRunStatus::Failed;
	
	data.ElapsedTime += DeltaTime;
	
	// 쿨다운 중간 시점에 한 번 공격
	if (!data.bAttacked && data.ElapsedTime >= data.AttackCooldown * 0.3f)
	{
		const float dist = FVector::Dist(owner->GetActorLocation(), data.Target->GetActorLocation());
		
		if (dist <= data.AttackRadius)
		{
			FDamageInfo info;
			info.DamageAmount = data.Damage;
			info.DamageCauser = owner;
			
			IDamageable::Execute_ReceiveDamage(data.Target, info);
		}
		data.bAttacked = true;
	}
	
	// 쿨다운 완료 -> Succeeded로 Combat 루프 재진입
	if (data.ElapsedTime >= data.AttackCooldown) return EStateTreeRunStatus::Succeeded;
	return EStateTreeRunStatus::Running;
}
