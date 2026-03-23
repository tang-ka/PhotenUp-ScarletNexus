// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossIdle.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
 

// EnterState: 대기 시작
// - 랜덤으로 대기 시간을 정하고
// - 걷기 속도를 느리게 설정
EStateTreeRunStatus FSTTask_BossIdle::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	// 대기 시간 초기화
	InstanceData.ElapsedTime = 0.f;
 
	// 랜덤 대기 시간 결정 (예: 1.5초 ~ 3초 사이)
	InstanceData.WaitDuration = FMath::FRandRange(WaitTimeRange.X, WaitTimeRange.Y);
 
	// 보스 캐릭터 가져오기
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 걷기 속도를 느리게 설정
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossIdle] 대기 시작 (%.1f초 동안)"), InstanceData.WaitDuration);
 
	return EStateTreeRunStatus::Running;
}
 
// Tick: 매 프레임 실행
// - 플레이어를 부드럽게 바라보기
// - 적정 거리를 유지하며 천천히 이동
// - 대기 시간이 끝나면 Succeeded 반환 (다음 공격으로)
EStateTreeRunStatus FSTTask_BossIdle::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	// 시간 경과 체크 — 대기 시간이 끝나면 공격으로 전환
	InstanceData.ElapsedTime += DeltaTime;
	if (InstanceData.ElapsedTime >= InstanceData.WaitDuration)
	{
		return EStateTreeRunStatus::Succeeded;
	}
 
	// 보스 캐릭터 가져오기
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
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
 
	// 플레이어를 향한 목표 회전값 계산
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(BossLocation, PlayerLocation);
 
	// Yaw(좌우 회전)만 사용 (보스가 위아래로 기울어지면 안 되니까)
	const FRotator TargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
 
	// 현재 회전에서 목표 회전으로 부드럽게 보간
	const FRotator CurrentRotation = BossChar->GetActorRotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		RotationSpeed	// 초당 회전 속도 (도)
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
			// 적정 거리: 플레이어 주위를 옆으로 서성이기
			// 보스의 오른쪽 방향으로 천천히 이동 (원을 그리는 느낌)
			const FVector StrafeDirection = BossChar->GetActorRightVector();
			BossChar->AddMovementInput(StrafeDirection, 0.6f);
		}
	}
 
	return EStateTreeRunStatus::Running;
}
 
// ExitState: 대기 끝, 정리
// - 다음 공격을 위해 이동 속도를 원래대로 복구
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
 
	UE_LOG(LogTemp, Log, TEXT("[BossIdle] 대기 종료 (경과: %.1f초)"), InstanceData.ElapsedTime);
}