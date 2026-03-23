// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_CloneRush.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
 
EStateTreeRunStatus FSTTask_CloneRush::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	Data.AttackPhase = ECloneRushPhase::WindUp;
	Data.PhaseTimer = 0.f;
	Data.bDamageApplied = false;
 
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss)
	{
		UE_LOG(LogTemp, Error, TEXT("[CloneRush] ContextActor가 없습니다."));
		return EStateTreeRunStatus::Failed;
	}
 
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("[CloneRush] 플레이어를 찾을 수 없습니다."));
		return EStateTreeRunStatus::Failed;
	}
 
	// 플레이어를 향한 방향 계산 (고정)
	Data.StartLocation = Boss->GetActorLocation();
	Data.RushDirection = (Player->GetActorLocation() - Data.StartLocation).GetSafeNormal2D();
	Data.TargetLocation = Data.StartLocation + Data.RushDirection * RushDistance;
 
	// 플레이어를 향해 회전
	Boss->SetActorRotation(Data.RushDirection.Rotation());
 
	UE_LOG(LogTemp, Log, TEXT("[CloneRush] 돌진 준비 - 방향: %s, 거리: %.0f"),
		*Data.RushDirection.ToString(), RushDistance);
 
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_CloneRush::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	Data.PhaseTimer += DeltaTime;
 
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	switch (Data.AttackPhase)
	{
	case ECloneRushPhase::WindUp:
		{
			// 준비 시간 대기 (살짝 뒤로 몸 당기는 느낌)
			if (Data.PhaseTimer >= WindUpDuration)
			{
				Data.AttackPhase = ECloneRushPhase::Rushing;
				Data.PhaseTimer = 0.f;
				UE_LOG(LogTemp, Log, TEXT("[CloneRush] 돌진 시작!"));
			}
		}
		break;
 
	case ECloneRushPhase::Rushing:
		{
			// 고속 이동
			const FVector CurrentLoc = Boss->GetActorLocation();
			const FVector NewLoc = CurrentLoc + Data.RushDirection * RushSpeed * DeltaTime;
 
			// 목표 지점 도달 체크
			const float DistanceTraveled = FVector::Dist2D(Data.StartLocation, NewLoc);
 
			if (DistanceTraveled >= RushDistance)
			{
				// 목표 도달 - 최종 위치로 설정
				Boss->SetActorLocation(Data.TargetLocation);
 
				Data.AttackPhase = ECloneRushPhase::Recovery;
				Data.PhaseTimer = 0.f;
 
				UE_LOG(LogTemp, Log, TEXT("[CloneRush] 돌진 완료 → 회복 중"));
			}
			else
			{
				Boss->SetActorLocation(NewLoc);
			}
 
			// 돌진 중 데미지 판정 (매 프레임)
			if (!Data.bDamageApplied)
			{
				ApplyRushDamage(Boss, Data.RushDirection);
				// 한번 맞으면 더 이상 안 맞게
				// Data.bDamageApplied = true; 
				// → 관통 데미지를 원하면 이 줄 주석 유지
			}
		}
		break;
 
	case ECloneRushPhase::Recovery:
		{
			// 돌진 후 회복 대기 (보스가 잠깐 멈춤)
			if (Data.PhaseTimer >= RecoveryDuration)
			{
				Data.AttackPhase = ECloneRushPhase::Done;
				UE_LOG(LogTemp, Log, TEXT("[CloneRush] 공격 완료"));
				return EStateTreeRunStatus::Succeeded;
			}
		}
		break;
 
	case ECloneRushPhase::Done:
		return EStateTreeRunStatus::Succeeded;
 
	default:
		break;
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_CloneRush::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	// 필요시 보스 속도 복원 등
}
 
void FSTTask_CloneRush::ApplyRushDamage(
	AActor* BossActor,
	const FVector& RushDir) const
{
	// 보스 현재 위치 기준으로 전방 판정
	const FVector BossLoc = BossActor->GetActorLocation();
 
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(BossActor);
 
	BossActor->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		BossLoc,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(RushWidth),
		QueryParams
	);
 
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor || HitActor == BossActor)
		{
			continue;
		}
 
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if (!HitCharacter)
		{
			continue;
		}
 
		FDamageEvent DamageEvent;
		HitCharacter->TakeDamage(RushDamage, DamageEvent, nullptr, BossActor);
 
		UE_LOG(LogTemp, Log, TEXT("[CloneRush] %s에게 %.0f 데미지!"),
			*HitCharacter->GetName(), RushDamage);
 
		// 넉백 (돌진 방향으로)
		if (UCharacterMovementComponent* Movement = HitCharacter->GetCharacterMovement())
		{
			const FVector KnockbackDir = (RushDir + FVector(0.f, 0.f, 0.3f)).GetSafeNormal();
			Movement->AddImpulse(KnockbackDir * KnockbackForce, true);
		}
	}
}
