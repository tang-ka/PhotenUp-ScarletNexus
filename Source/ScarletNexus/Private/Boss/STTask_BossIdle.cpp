// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossIdle.h"
#include "Interface/Damageable.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
 


// EnterState: 대기 시작

EStateTreeRunStatus FSTTask_BossIdle::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// ★ 사망 가드 — 죽은 상태면 Running으로 멈춤 (Failed는 무한루프 유발)
	if (IDamageable::Execute_IsDead(BossChar))
	{
		UE_LOG(LogTemp, Log, TEXT("[BossIdle] 보스 사망 상태 — 대기 중지"));
		return EStateTreeRunStatus::Running;
	}
 
	// 대기 시간 초기화
	InstanceData.ElapsedTime = 0.f;
 
	// 랜덤 대기 시간 결정
	InstanceData.WaitDuration = FMath::FRandRange(WaitTimeRange.X, WaitTimeRange.Y);
 
	// 걷기 속도를 느리게 설정
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossIdle] 대기 시작 (%.1f초 동안)"), InstanceData.WaitDuration);
 
	return EStateTreeRunStatus::Running;
}
 

// Tick

EStateTreeRunStatus FSTTask_BossIdle::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// ★ 사망 가드 — Tick에서도 체크
	if (IDamageable::Execute_IsDead(BossChar))
	{
		return EStateTreeRunStatus::Running;
	}
 
	// 시간 경과 체크 — 대기 시간이 끝나면 공격으로 전환
	InstanceData.ElapsedTime += DeltaTime;
	if (InstanceData.ElapsedTime >= InstanceData.WaitDuration)
	{
		UE_LOG(LogTemp, Log, TEXT("[BossIdle] 대기 종료 (경과: %.1f초)"), InstanceData.ElapsedTime);
		return EStateTreeRunStatus::Succeeded;
	}
 
	// 플레이어 캐릭터 가져오기
	const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(BossChar->GetWorld(), 0);
	if (!PlayerChar)
	{
		return EStateTreeRunStatus::Running;
	}
 
	
	// 1. 플레이어를 부드럽게 바라보기
	
	const FVector BossLocation = BossChar->GetActorLocation();
	const FVector PlayerLocation = PlayerChar->GetActorLocation();
 
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(BossLocation, PlayerLocation);
	const FRotator TargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	const FRotator CurrentRotation = BossChar->GetActorRotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		RotationSpeed
	);
	BossChar->SetActorRotation(NewRotation);
 
	
	// 2. 적정 거리 유지하며 천천히 이동
	
	const float Distance = FVector::Dist(BossLocation, PlayerLocation);
 
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		if (Distance > PreferredDistance + DistanceTolerance)
		{
			// 너무 멀면: 플레이어 방향으로 천천히 접근
			const FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
			BossChar->AddMovementInput(Direction, 1.f);
		}
		else if (Distance < PreferredDistance - DistanceTolerance)
		{
			// 너무 가까우면: 뒤로 천천히 물러남
			const FVector Direction = (BossLocation - PlayerLocation).GetSafeNormal();
			BossChar->AddMovementInput(Direction, 0.5f);
		}
		else
		{
			// 적정 거리: 플레이어 주변을 옆으로 서성임
			const FVector ToPlayer = (PlayerLocation - BossLocation).GetSafeNormal();
			const FVector StrafeDir = FVector(-ToPlayer.Y, ToPlayer.X, 0.f);
			BossChar->AddMovementInput(StrafeDir, 0.6f);
		}
	}
 
	return EStateTreeRunStatus::Running;
}
 

// ExitState: 대기 끝

void FSTTask_BossIdle::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return;
	}
 
	// 이동 속도를 기본값으로 복구
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 500.f;
	}
}