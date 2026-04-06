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
	data.MontageLength = 0.f;
	
	// IDamageable 적용 체크
	bool isDamageable = DamageableHelpers::IsDamageable(data.Target);
	PRINTLOG_GT(TEXT("Target: %s | IsDamageable: %s"), data.Target ? *data.Target->GetName() : TEXT("NULL"),
		isDamageable ? TEXT("true") : TEXT("false"));
	if (!data.Target || !isDamageable) return EStateTreeRunStatus::Failed;
	
	// 타겟 방향으로 회전
	ACharacter* owner = Cast<ACharacter>(Context.GetOwner());
	PRINTLOG_GT(TEXT("Owner: %s"), owner ? *owner->GetClass()->GetName() : TEXT("NULL"));
	if (owner)
	{
		FVector dir = (data.Target->GetActorLocation() - owner->GetActorLocation()).GetSafeNormal2D();
		if (!dir.IsNearlyZero())
		{
			owner->SetActorRotation(dir.Rotation());
		}
	}
	else return EStateTreeRunStatus::Failed;

	// 등록된 몽타주가 없으면 실패
	if (data.AttackMontageList.IsEmpty()) return EStateTreeRunStatus::Failed;

	UAnimInstance* animInst = owner->GetMesh() ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if (!animInst) return EStateTreeRunStatus::Failed;

	// 유효한 몽타주만 필터링
	TArray<UAnimMontage*> validMontageList;
	for (UAnimMontage* montage : data.AttackMontageList)
	{
		if (montage) validMontageList.AddUnique(montage);
	}
	if (validMontageList.IsEmpty()) return EStateTreeRunStatus::Failed;

	// 랜덤 선택
	const int32 idx = FMath::RandRange(0, validMontageList.Num() - 1);
	UAnimMontage* selectMontage = validMontageList[idx];

	// 재생, 길이 저장
	animInst->Montage_Play(selectMontage, data.PlayRate);
	data.MontageLength = selectMontage->GetPlayLength() / data.PlayRate;
	PRINTLOG_GT(TEXT("PlayMontage Length: %.2f, Montage: %s"), data.MontageLength, *selectMontage->GetName());
	
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
			//partyMember
			IDamageable::Execute_ReceiveDamage(data.Target, info);
		}
		data.bAttacked = true;
	}
	
	// 쿨다운 완료 -> Succeeded로 Combat 루프 재진입
	if (data.ElapsedTime >= data.AttackCooldown) return EStateTreeRunStatus::Succeeded;
	return EStateTreeRunStatus::Running;
}

void FPCTask_Attack::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 상태 전환 등으로 중단되었을 때 몽타주 강제 정지
	ACharacter* owner = Cast<ACharacter>(Context.GetOwner());
	if (!owner) return;

	UAnimInstance* animInst = owner->GetMesh() ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if (!animInst) return;

	// 현재 재생중인 몽타주가 있으면 중단
	if (UAnimMontage* currMontage = animInst->GetCurrentActiveMontage())
	{
		animInst->Montage_Stop(0.2f, currMontage);
	}
}
