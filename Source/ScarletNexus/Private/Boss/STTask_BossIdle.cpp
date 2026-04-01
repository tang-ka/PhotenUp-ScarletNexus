// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossIdle.h"
#include "Interface/Damageable.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
 

// EnterState

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
 
	if (IDamageable::Execute_IsDead(BossChar))
	{
		UE_LOG(LogTemp, Log, TEXT("[BossIdle] 보스 사망 상태 — 대기 중지"));
		return EStateTreeRunStatus::Running;
	}
 
	InstanceData.ElapsedTime = 0.f;
	InstanceData.WaitDuration = FMath::FRandRange(WaitTimeRange.X, WaitTimeRange.Y);
 
	// 회피 상태 초기화
	InstanceData.bIsEvadeTeleporting = false;
	InstanceData.EvadeTeleportTimer = 0.f;
 
	// 순찰 텔레포트 초기화 — 진입 직후 빠르게 첫 텔레포트
	InstanceData.bIsPatrolTeleporting = false;
	InstanceData.PatrolTeleportTimer = 0.f;
	InstanceData.PatrolCooldownTimer = FMath::FRandRange(0.3f, 0.8f);
 
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossIdle] 대기 시작 (%.1f초 동안)"), InstanceData.WaitDuration);
 
	return EStateTreeRunStatus::Running;
}
 
// ============================================================
// Tick
// ============================================================
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
 
	if (IDamageable::Execute_IsDead(BossChar))
	{
		return EStateTreeRunStatus::Running;
	}
 
	// 쿨다운 감소
	InstanceData.EvadeCooldownTimer -= DeltaTime;
	InstanceData.PatrolCooldownTimer -= DeltaTime;
 
	// ══════════════════════════════════════
	// 1) 회피 텔레포트 진행 중
	// ══════════════════════════════════════
	if (InstanceData.bIsEvadeTeleporting)
	{
		InstanceData.EvadeTeleportTimer += DeltaTime;
 
		if (InstanceData.EvadeTeleportTimer >= EvadeVanishDuration)
		{
			BossChar->SetActorLocation(InstanceData.EvadeTeleportTarget);
			BossChar->SetActorHiddenInGame(false);
			BossChar->SetActorEnableCollision(true);
 
			if (const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(
					BossChar->GetWorld(), 0))
			{
				const FVector Dir = (Player->GetActorLocation()
					- InstanceData.EvadeTeleportTarget).GetSafeNormal();
				BossChar->SetActorRotation(FRotator(0.f, Dir.Rotation().Yaw, 0.f));
			}
 
			InstanceData.bIsEvadeTeleporting = false;
			InstanceData.EvadeCooldownTimer = EvadeCooldown;
		}
		return EStateTreeRunStatus::Running;
	}
 
	// ══════════════════════════════════════
	// 2) 순찰 텔레포트 진행 중
	// ══════════════════════════════════════
	if (InstanceData.bIsPatrolTeleporting)
	{
		InstanceData.PatrolTeleportTimer += DeltaTime;
 
		if (InstanceData.PatrolTeleportTimer >= PatrolVanishDuration)
		{
			BossChar->SetActorLocation(InstanceData.PatrolTeleportTarget);
			BossChar->SetActorHiddenInGame(false);
			BossChar->SetActorEnableCollision(true);
 
			if (const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(
					BossChar->GetWorld(), 0))
			{
				const FVector Dir = (Player->GetActorLocation()
					- InstanceData.PatrolTeleportTarget).GetSafeNormal();
				BossChar->SetActorRotation(FRotator(0.f, Dir.Rotation().Yaw, 0.f));
			}
 
			InstanceData.bIsPatrolTeleporting = false;
			InstanceData.PatrolCooldownTimer = FMath::FRandRange(
				PatrolIntervalRange.X, PatrolIntervalRange.Y);
 
			UE_LOG(LogTemp, Log, TEXT("[BossIdle] 순찰 텔레포트 완료 → %s"),
				*InstanceData.PatrolTeleportTarget.ToString());
		}
		return EStateTreeRunStatus::Running;
	}
 
	// ══════════════════════════════════════
	// 3) 대기 시간 체크
	// ══════════════════════════════════════
	InstanceData.ElapsedTime += DeltaTime;
	if (InstanceData.ElapsedTime >= InstanceData.WaitDuration)
	{
		UE_LOG(LogTemp, Log, TEXT("[BossIdle] 대기 종료 (경과: %.1f초)"), InstanceData.ElapsedTime);
		return EStateTreeRunStatus::Succeeded;
	}
 
	const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(BossChar->GetWorld(), 0);
	if (!PlayerChar)
	{
		return EStateTreeRunStatus::Running;
	}
 
	const FVector BossLocation = BossChar->GetActorLocation();
	const FVector PlayerLocation = PlayerChar->GetActorLocation();
	const float Distance = FVector::Dist(BossLocation, PlayerLocation);
 
	// ══════════════════════════════════════
	// 4) 회피 텔레포트 발동 (플레이어가 너무 가까울 때)
	// ══════════════════════════════════════
	if (Distance < EvadeTriggerDistance && InstanceData.EvadeCooldownTimer <= 0.f)
	{
		FVector BackDir = (BossLocation - PlayerLocation).GetSafeNormal();
		FVector TeleportTarget = BossLocation + BackDir * EvadeTeleportDistance;
 
		if (const UNavigationSystemV1* NavSys =
				FNavigationSystem::GetCurrent<UNavigationSystemV1>(BossChar->GetWorld()))
		{
			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(TeleportTarget, NavLoc, FVector(300.f)))
			{
				TeleportTarget = NavLoc.Location;
			}
		}
 
		InstanceData.bIsEvadeTeleporting = true;
		InstanceData.EvadeTeleportTimer = 0.f;
		InstanceData.EvadeTeleportTarget = TeleportTarget;
 
		BossChar->SetActorHiddenInGame(true);
		BossChar->SetActorEnableCollision(false);
 
		UE_LOG(LogTemp, Log, TEXT("[BossIdle] 회피 텔레포트! → %s"), *TeleportTarget.ToString());
		return EStateTreeRunStatus::Running;
	}
 
	// ══════════════════════════════════════
	// 5) 순찰 텔레포트 발동 (쿨다운 끝나면 랜덤 위치로)
	// ══════════════════════════════════════
	if (InstanceData.PatrolCooldownTimer <= 0.f)
	{
		const float RandAngle = FMath::FRandRange(0.f, 360.f);
		const float RandDist = FMath::FRandRange(PatrolMinDistance, PatrolMaxDistance);
		const FVector RandDir = FRotator(0.f, RandAngle, 0.f).Vector();
		FVector TeleportTarget = PlayerLocation + RandDir * RandDist;
 
		// 높이는 보스 현재 높이 유지
		TeleportTarget.Z = BossLocation.Z;
 
		// NavMesh 보정
		if (const UNavigationSystemV1* NavSys =
				FNavigationSystem::GetCurrent<UNavigationSystemV1>(BossChar->GetWorld()))
		{
			FNavLocation NavLoc;
			if (NavSys->ProjectPointToNavigation(TeleportTarget, NavLoc, FVector(500.f)))
			{
				TeleportTarget = NavLoc.Location;
			}
			else
			{
				// NavMesh에 유효한 위치 못 찾으면 이번엔 스킵
				InstanceData.PatrolCooldownTimer = 0.5f;
				return EStateTreeRunStatus::Running;
			}
		}
 
		InstanceData.bIsPatrolTeleporting = true;
		InstanceData.PatrolTeleportTimer = 0.f;
		InstanceData.PatrolTeleportTarget = TeleportTarget;
 
		BossChar->SetActorHiddenInGame(true);
		BossChar->SetActorEnableCollision(false);
 
		return EStateTreeRunStatus::Running;
	}
 
	// ══════════════════════════════════════
	// 6) 텔레포트 사이 — 플레이어를 바라보며 걷기
	// ══════════════════════════════════════
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(BossLocation, PlayerLocation);
	const FRotator TargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	const FRotator NewRotation = FMath::RInterpConstantTo(
		BossChar->GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
	BossChar->SetActorRotation(NewRotation);
 
	// 거리에 따라 이동
	if (BossChar->GetCharacterMovement())
	{
		const FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
 
		if (Distance > PreferredDistance + DistanceTolerance)
		{
			// 너무 멀면 다가감
			BossChar->AddMovementInput(Direction, 1.f);
		}
		else if (Distance < PreferredDistance - DistanceTolerance)
		{
			// 너무 가까우면 뒤로
			BossChar->AddMovementInput(-Direction, 0.5f);
		}
		else
		{
			// 적정 거리: 옆으로 서성임
			const FVector StrafeDir = FVector(-Direction.Y, Direction.X, 0.f);
			BossChar->AddMovementInput(StrafeDir, 0.6f);
		}
	}
 
	return EStateTreeRunStatus::Running;
}
 
// ============================================================
// ExitState
// ============================================================
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
 
	// 텔레포트 중 상태 전환 시 안전장치
	if (InstanceData.bIsEvadeTeleporting || InstanceData.bIsPatrolTeleporting)
	{
		BossChar->SetActorHiddenInGame(false);
		BossChar->SetActorEnableCollision(true);
		InstanceData.bIsEvadeTeleporting = false;
		InstanceData.bIsPatrolTeleporting = false;
	}
 
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 500.f;
	}
}
