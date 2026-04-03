// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyTask_Attack.h"

#include "StateTreeExecutionContext.h"
#include "PartyAI/PartyMemberBase.h"
#include "Player/PlayerCharacterBase.h"

EStateTreeRunStatus FEnemyTask_Attack::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition) const
{
	auto& data = Context.GetInstanceData(*this);
	data.ElapsedTime = 0.0f;
	data.bDamageApplied = false;
	
	if (!data.Target) return EStateTreeRunStatus::Failed;
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FEnemyTask_Attack::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	auto& data = Context.GetInstanceData(*this);
	AActor* owner = Cast<AActor>(Context.GetOwner());
	if (!owner || !data.Target) return EStateTreeRunStatus::Failed;
	
	data.ElapsedTime += DeltaTime;
	
	// 쿨다운 중 특정 시점에 한 번 데미지 적용
	if (!data.bDamageApplied && data.ElapsedTime >= data.AttackCooldown * data.DamageTimingRatio)
	{
		const float dist = FVector::Dist(owner->GetActorLocation(), data.Target->GetActorLocation());
		
		if (dist <= data.AttackRadius)
		{
			// 파티 맴버 대상
			if (APartyMemberBase* partyMember = Cast<APartyMemberBase>(data.Target))
			{
				partyMember->TakeDamage_Party(data.AtkDmg);
			}
			// 플레이어 대상 데미지 처리 추가
			else if (APlayerCharacterBase* player = Cast<APlayerCharacterBase>(data.Target))
			{
				FDamageInfo info;
				info.DamageAmount = data.AtkDmg;
				info.DamageCauser = owner;
				player->ReceiveDamage_Implementation(info);
			}
		}
		
		data.bDamageApplied = true;
		
		// TODO: 공격 애니메이션 재생
		
	}
	
	// 쿨다운 완료 -> Succeeded로 Combat 루프 재진입
	if (data.ElapsedTime >= data.AttackCooldown) return EStateTreeRunStatus::Succeeded;
	
	return EStateTreeRunStatus::Running;
}
