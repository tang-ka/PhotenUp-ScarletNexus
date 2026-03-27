// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossDeath.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
 
EStateTreeRunStatus FSTTask_BossDeath::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;
 
	Data.Timer = 0.f;
	Data.bDeathStarted = true;
 
	// 이동 정지
	if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
 
	// 콜리전 비활성화
	if (UCapsuleComponent* Capsule = Boss->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
 
	// 사망 몽타주 추가해야함.
	
	// if (DeathMontage) Boss->PlayAnimMontage(DeathMontage);
 
	UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 사망 State 진입 — 연출 시작"));
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossDeath::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;
 
	Data.Timer += DeltaTime;
 
	// 사망 연출 대기
	if (Data.Timer >= DeathDuration && Data.Timer < DeathDuration + HideDelay)
	{
		// 연출 완료 후 대기 중
	}
 
	// 숨김 처리
	if (Data.Timer >= DeathDuration + HideDelay)
	{
		Boss->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 사망 완료 — 액터 숨김"));
 
		// Succeeded를 반환하지만 Death는 최종 State이므로
		// StateTree에서 Transition 없이 끝남
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 
