// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossTeleport.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
 
EStateTreeRunStatus FSTTask_BossTeleport::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime = 0.f;
	InstanceData.bTeleportComplete = false;
 
	AActor* BossActor = InstanceData.ContextActor;
	if (!BossActor)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 플레이어 액터 가져오기
	const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(BossActor->GetWorld(), 0);
	if (!PlayerChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// Phase 3에서는 잔상 텔레포트를 강제 사용
	ETeleportStrategy EffectiveStrategy = Strategy;
	if (InstanceData.CurrentPhase == EBossPhase::Phase3_Cutscene)
	{
		EffectiveStrategy = ETeleportStrategy::RandomWithAfterimage;
	}
 
	// 목표 위치 계산
	InstanceData.TargetLocation = CalculateTargetLocation(BossActor, PlayerChar, EffectiveStrategy);
 
	// TODO: 사라짐 이펙트 재생
	// if (TeleportVFX)
	// {
	//     UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	//         BossActor->GetWorld(), TeleportVFX, BossActor->GetActorLocation());
	// }
 
	// TODO: 보스 메시 비가시화 + 콜리전 비활성화
	// BossActor->SetActorHiddenInGame(true);
	// BossActor->SetActorEnableCollision(false);
	
 
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossTeleport::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime += DeltaTime;
 
	// 텔레포트 Duration 경과 후 실제 이동 수행
	if (!InstanceData.bTeleportComplete && InstanceData.ElapsedTime >= TeleportDuration)
	{
		AActor* BossActor = InstanceData.ContextActor;
		if (BossActor)
		{
			// 실제 위치 이동
			BossActor->SetActorLocation(InstanceData.TargetLocation);
 
			// 플레이어를 향해 회전
			if (const ACharacter* PlayerChar = 
				UGameplayStatics::GetPlayerCharacter(BossActor->GetWorld(), 0))
			{
				const FVector Direction = (PlayerChar->GetActorLocation() - 
					InstanceData.TargetLocation).GetSafeNormal();
				BossActor->SetActorRotation(Direction.Rotation());
			}
 
			// TODO: 등장 이펙트 재생
			// TODO: 메시 재가시화 + 콜리전 재활성화
		}
 
		InstanceData.bTeleportComplete = true;
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_BossTeleport::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	// 안전장치: 텔레포트 도중 중단되었으면 보스를 다시 보이게
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.bTeleportComplete)
	{
		if (AActor* BossActor = InstanceData.ContextActor)
		{
			// BossActor->SetActorHiddenInGame(false);
			// BossActor->SetActorEnableCollision(true);
		}
	}
}
 
FVector FSTTask_BossTeleport::CalculateTargetLocation(
	const AActor* BossActor,
	const AActor* PlayerActor,
	ETeleportStrategy InStrategy) const
{
	const FVector PlayerLoc = PlayerActor->GetActorLocation();
	const FVector PlayerForward = PlayerActor->GetActorForwardVector();
	const FVector PlayerRight = PlayerActor->GetActorRightVector();
	FVector Result = PlayerLoc;
 
	switch (InStrategy)
	{
	case ETeleportStrategy::BehindPlayer:
		// 플레이어 뒤쪽
		Result = PlayerLoc - PlayerForward * OffsetDistance;
		break;
 
	case ETeleportStrategy::FlankPlayer:
		{
			// 좌우 랜덤 선택
			const float Side = FMath::RandBool() ? 1.f : -1.f;
			Result = PlayerLoc + PlayerRight * (Side * OffsetDistance);
		}
		break;
 
	case ETeleportStrategy::RandomWithAfterimage:
		{
			// Phase 3 전용: 랜덤 각도로 이동 (잔상 여러 개 남김)
			const float RandomAngle = FMath::FRandRange(0.f, 360.f);
			const FVector RandomDir = FVector(
				FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
				FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
				0.f
			);
			Result = PlayerLoc + RandomDir * OffsetDistance;
 
			// TODO: 잔상(Afterimage) 생성
			// 텔레포트 경로를 따라 반투명 잔상 메시를 여러 개 스폰
		}
		break;
 
	case ETeleportStrategy::AerialAbove:
		// 플레이어 위쪽 (내려찍기 연계용)
		Result = PlayerLoc + FVector(0.f, 0.f, 500.f);
		break;
	}
 
	// NavMesh 위의 유효한 위치로 보정 (공중 제외)
	if (InStrategy != ETeleportStrategy::AerialAbove)
	{
		if (const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(BossActor->GetWorld()))
		{
			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(Result, NavLoc, FVector(200.f, 200.f, 200.f)))
			{
				Result = NavLoc.Location;
			}
		}
	}
 
	return Result;
}
 