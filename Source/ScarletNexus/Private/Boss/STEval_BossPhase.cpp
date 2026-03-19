// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STEval_BossPhase.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
 
#include "Boss/IBossCharacterInterface.h"
 
void FSTEval_BossPhase::TreeStart(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	InstanceData.CurrentPhase = EBossPhase::Phase1_Probe;
	InstanceData.HPRatio = 1.f;
	InstanceData.StaggerRatio = 0.f;
	InstanceData.TimeSinceLastAttack = 0.f;
	InstanceData.bIsPhaseTransitioning = false;
	InstanceData.bBrainCrushAvailable = false;
}
 
 void FSTEval_BossPhase::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
 {
 	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
  
 	AActor* BossActor = InstanceData.ContextActor;
 	if (!BossActor)
 	{
 		return;
 	}
 
	 // 보스 상태 조회 (IBossCharacterInterface)
	if (BossActor->GetClass()->ImplementsInterface(UBossCharacterInterface::StaticClass()))
	{
		InstanceData.HPRatio = IBossCharacterInterface::Execute_GetHPRatio(BossActor);
		InstanceData.StaggerRatio = IBossCharacterInterface::Execute_GetStaggerRatio(BossActor);
	}
 
	 // 플레이어 거리 및 시야 계산
	 if (const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(BossActor->GetWorld(), 0))
	 {
	 	InstanceData.DistanceToPlayer = FVector::Dist(
	 		BossActor->GetActorLocation(),
	 		PlayerChar->GetActorLocation()
	 	);
 
		 //간단한 시야 판정 (전방 120도 내)
	 	const FVector ToPlayer = (PlayerChar->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal();
	 	const FVector Forward = BossActor->GetActorForwardVector();
	 	const float DotProduct = FVector::DotProduct(Forward, ToPlayer);
	 	InstanceData.bPlayerInSight = (DotProduct > 0.5f); // cos(60°) = 0.5
	 }
 
	//  페이즈 전환 판정
	const EBossPhase NewPhase = DeterminePhase(InstanceData.HPRatio);
	if (NewPhase != InstanceData.CurrentPhase)
	 {
	 	InstanceData.bIsPhaseTransitioning = true;
	 	InstanceData.CurrentPhase = NewPhase;
	 	// 실제로는 페이즈 전환 몽타주 재생 후 false로 돌려야 함
	 }
 
	 // 브레인 크래시 가능 여부
	InstanceData.bBrainCrushAvailable = (InstanceData.StaggerRatio >= 0.8f);
 
	 // 공격 타이머 갱신
	InstanceData.TimeSinceLastAttack += DeltaTime;
 }
 
 EBossPhase FSTEval_BossPhase::DeterminePhase(float HPRatio) const
 {
 	if (HPRatio <= 0.35f)
 	{
 		return EBossPhase::Phase3_Awakening;
 	}
 	if (HPRatio <= 0.70f)
 	{
 		return EBossPhase::Phase2_Assault;
 	}
 	return EBossPhase::Phase1_Probe;
 }
