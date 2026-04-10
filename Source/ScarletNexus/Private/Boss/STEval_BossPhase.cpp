// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STEval_BossPhase.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/Damageable.h"
#include "Interface/DamageableHelper.h"
#include "Boss/BossGameplayTags.h"
 
void FSTEval_BossPhase::TreeStart(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	InstanceData.CurrentPhase = EBossPhase::Phase1;
	InstanceData.HPRatio = 1.f;
	InstanceData.TimeSinceLastAttack = 0.f;
	InstanceData.bIsPhaseTransitioning = false;
}
 
void FSTEval_BossPhase::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	AActor* BossActor = InstanceData.ContextActor;
	if (!BossActor)
	{
		return;
	}
 
	// 0. 사망 체크 — Evaluator에서 보내야 확실히 처리됨
	if (DamageableHelpers::IsDead(BossActor))
	{
		if (!InstanceData.bDeathEventSent)
		{
			InstanceData.bDeathEventSent = true;
			Context.SendEvent(BossTags::Event_Death);
		}
		return;
	}
 
	
	// 1. 보스 상태 조회
	// HP는 IDamageable로 조회
	if (BossActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		InstanceData.HPRatio = IDamageable::Execute_GetHPPercent(BossActor);
	}
 
	// 2. 플레이어 거리 및 시야 계산
	if (const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(BossActor->GetWorld(), 0))
	{
		InstanceData.DistanceToPlayer = FVector::Dist(
			BossActor->GetActorLocation(),
			PlayerChar->GetActorLocation()
		);
 
		// 간단한 시야 판정 (전방 120도 내)
		const FVector ToPlayer = (PlayerChar->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal();
		const FVector Forward = BossActor->GetActorForwardVector();
		const float DotProduct = FVector::DotProduct(Forward, ToPlayer);
		InstanceData.bPlayerInSight = (DotProduct > 0.5f); // cos(60°) = 0.5
	}
 
	// 3. 페이즈 전환 판정
	const EBossPhase NewPhase = DeterminePhase(InstanceData.HPRatio);
	if (NewPhase != InstanceData.CurrentPhase)
	{
		InstanceData.bIsPhaseTransitioning = true;
		InstanceData.CurrentPhase = NewPhase;
		// 실제로는 페이즈 전환 몽타주 재생 후 false로 돌려야 함
	}
 
	// 4. 공격 타이머 갱신
	InstanceData.TimeSinceLastAttack += DeltaTime;
}
 
EBossPhase FSTEval_BossPhase::DeterminePhase(float HPRatio) const
{
	if (HPRatio <= 0.25f)
	{
		return EBossPhase::Phase3_Cutscene;
	}
	if (HPRatio <= 0.50f)
	{
		return EBossPhase::Phase2_Enhanced;
	}
	if (HPRatio <= 0.70f)
	{
		return EBossPhase::Phase2;
	}
	return EBossPhase::Phase1;
}