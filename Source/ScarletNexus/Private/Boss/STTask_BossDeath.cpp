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
 
	// 시작 위치 저장
	Data.StartLocation = Boss->GetActorLocation();
 
	// 이동 정지
	if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
 
	// 콜리전 비활성화 (바닥 뚫고 내려가야 하므로)
	if (UCapsuleComponent* Capsule = Boss->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
 
	UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 사망 State 진입 — 가라앉기 시작"));
	return EStateTreeRunStatus::Running;
	
}
 
EStateTreeRunStatus FSTTask_BossDeath::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;

	Data.Timer += DeltaTime;

	// 1. 잠깐 멈춤 구간 
	if (Data.Timer < PauseBeforeSink)
	{
		return EStateTreeRunStatus::Running;
	}

	// 2. 가라앉기 연출
	const float SinkTimer = Data.Timer - PauseBeforeSink;
    
	if (SinkTimer < DeathDuration)
	{
		const float Alpha = FMath::Clamp(SinkTimer / DeathDuration, 0.f, 1.f);
		const float EasedAlpha = FMath::InterpEaseIn(0.f, 1.f, Alpha, 2.f);
		const float NewZ = Data.StartLocation.Z - (SinkDepth * EasedAlpha);

		FVector NewLocation = Data.StartLocation;
		NewLocation.Z = NewZ;
		Boss->SetActorLocation(NewLocation);
	}

	// 3. 완전히 가라앉은 후 숨김 처리
	if (SinkTimer >= DeathDuration + HideDelay)
	{
		Boss->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 사망 완료 — 액터 숨김"));
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
 
