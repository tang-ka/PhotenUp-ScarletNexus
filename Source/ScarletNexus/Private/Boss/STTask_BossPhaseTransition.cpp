// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossPhaseTransition.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
 
EStateTreeRunStatus FSTTask_BossPhaseTransition::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;
 
	Data.Timer = 0.f;
	Data.bTransitionStarted = true;
 
	// 전환 중 이동 정지
	if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
	}
 
	// 나중에 할것
	// 전환 연출 넣기
	// - 페이즈 전환 몽타주 재생
	// - VFX (기운 폭발, 색상 변경 등)
	// if (TransitionMontage) Boss->PlayAnimMontage(TransitionMontage);
 
	UE_LOG(LogTemp, Warning, TEXT("[BossPhaseTransition] 페이즈 전환 연출 시작 (%.1f초)"), TransitionDuration);
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossPhaseTransition::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
 
	Data.Timer += DeltaTime;
 
	if (Data.Timer >= TransitionDuration)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossPhaseTransition] 전환 연출 완료 → Idle 복귀"));
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_BossPhaseTransition::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return;
 
	// 이동 재활성화
	if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossPhaseTransition] ExitState — 이동 재활성화"));
}
