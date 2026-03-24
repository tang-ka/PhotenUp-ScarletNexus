// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossAttackExecutor.h"
#include "Boss/BossCloneActor.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "NavigationSystem.h"
 
// EnterState - 공격 선택 후 초기화

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	Data.PhaseTimer = 0.f;
	Data.bDamageApplied = false;
 
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss)
	{
		UE_LOG(LogTemp, Error, TEXT("[AttackExecutor] ContextActor가 없습니다."));
		return EStateTreeRunStatus::Failed;
	}
 
	// 랜덤 공격 선택
	Data.ActiveAttack = SelectAttack();
 
	switch (Data.ActiveAttack)
	{
	case EActiveAttackType::TeleportKick:
		return EnterTeleportKick(Data, Boss);
 
	case EActiveAttackType::CloneRush:
		return EnterCloneRush(Data, Boss);
 
	default:
		return EStateTreeRunStatus::Failed;
	}
}
 

// Tick - 선택된 공격 실행

EStateTreeRunStatus FSTTask_BossAttackExecutor::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
 
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	switch (Data.ActiveAttack)
	{
	case EActiveAttackType::TeleportKick:
		return TickTeleportKick(Data, Boss, DeltaTime);
 
	case EActiveAttackType::CloneRush:
		return TickCloneRush(Data, Boss, DeltaTime);
 
	default:
		return EStateTreeRunStatus::Failed;
	}
}
 

// ExitState - 안전장치

void FSTTask_BossAttackExecutor::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
 
	if (AActor* Boss = Data.ContextActor)
	{
		// 텔레포트 킥 도중 중단 시 보스 다시 보이게
		Boss->SetActorHiddenInGame(false);
		Boss->SetActorEnableCollision(true);
	}
 
	// 분신 정리
	if (Data.LeftClone && !Data.LeftClone->IsActorBeingDestroyed())
	{
		Data.LeftClone->Destroy();
	}
	if (Data.RightClone && !Data.RightClone->IsActorBeingDestroyed())
	{
		Data.RightClone->Destroy();
	}
	Data.LeftClone = nullptr;
	Data.RightClone = nullptr;
}
 

// 공격 선택 (랜덤)

EActiveAttackType FSTTask_BossAttackExecutor::SelectAttack() const
{
	const int32 Roll = FMath::RandRange(0, 1);
 
	switch (Roll)
	{
	case 0:
		UE_LOG(LogTemp, Log, TEXT("[AttackExecutor] 선택: 텔레포트 킥"));
		return EActiveAttackType::TeleportKick;
	case 1:
		UE_LOG(LogTemp, Log, TEXT("[AttackExecutor] 선택: 분신 돌진"));
		return EActiveAttackType::CloneRush;
	default:
		return EActiveAttackType::TeleportKick;
	}
}
 

// 텔레포트 킥 - Enter

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterTeleportKick(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	Data.TKPhase = ETKPhase::Vanishing;
 
	// 텔레포트 목표: 플레이어 뒤쪽
	const FVector PlayerLoc = Player->GetActorLocation();
	const FVector PlayerFwd = Player->GetActorForwardVector();
	FVector Target = PlayerLoc - PlayerFwd * TK_TeleportOffset;
 
	// NavMesh 보정
	if (const UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(Boss->GetWorld()))
	{
		FNavLocation NavLoc;
		if (NavSys->ProjectPointToNavigation(Target, NavLoc, FVector(300.f, 300.f, 300.f)))
		{
			Target = NavLoc.Location;
		}
	}
 
	Data.TKTarget = Target;
 
	// 보스 사라짐
	Boss->SetActorHiddenInGame(true);
	Boss->SetActorEnableCollision(false);
 
	UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 공격 시작 → %s"), *Target.ToString());
 
	return EStateTreeRunStatus::Running;
}
 

// 텔레포트 킥 - Tick

EStateTreeRunStatus FSTTask_BossAttackExecutor::TickTeleportKick(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
 
	switch (Data.TKPhase)
	{
	case ETKPhase::Vanishing:
		if (Data.PhaseTimer >= TK_VanishDuration)
		{
			Boss->SetActorLocation(Data.TKTarget);
 
			if (const ACharacter* Player =
				UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0))
			{
				const FVector Dir = (Player->GetActorLocation() - Data.TKTarget).GetSafeNormal();
				if (!Dir.IsNearlyZero())
				{
					Boss->SetActorRotation(Dir.Rotation());
				}
			}
 
			Data.TKPhase = ETKPhase::Teleporting;
			Data.PhaseTimer = 0.f;
			UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 텔레포트 완료"));
		}
		break;
 
	case ETKPhase::Teleporting:
		if (Data.PhaseTimer >= TK_AppearDelay)
		{
			Boss->SetActorHiddenInGame(false);
			Boss->SetActorEnableCollision(true);
 
			Data.TKPhase = ETKPhase::Kicking;
			Data.PhaseTimer = 0.f;
			UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 등장! 킥!"));
		}
		break;
 
	case ETKPhase::Kicking:
		if (!Data.bDamageApplied)
		{
			const FVector KickCenter = Boss->GetActorLocation() +
				Boss->GetActorForwardVector() * (TK_KickRadius * 0.5f);
			ApplyDamageInRadius(Boss, KickCenter, TK_KickRadius, TK_Damage,
				TK_KnockbackForce, Boss->GetActorForwardVector());
			Data.bDamageApplied = true;
		}
 
		if (Data.PhaseTimer >= TK_KickDuration)
		{
			UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 킥 완료"));
			return EStateTreeRunStatus::Succeeded;
		}
		break;
 
	default:
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 

// 분신 돌진 - Enter (본체 + 왼쪽/오른쪽 분신 스폰)

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterCloneRush(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	Data.CRPhase = ECRPhase::SpawnClones;
	Data.bDamageApplied = false;
	Data.bBossRushDamageApplied = false;
 
	// 플레이어를 향한 방향
	Data.CRStartLocation = Boss->GetActorLocation();
	Data.CRDirection = (Player->GetActorLocation() - Data.CRStartLocation).GetSafeNormal2D();
	Data.CRTargetLocation = Data.CRStartLocation + Data.CRDirection * CR_RushDistance;
 
	// 보스 회전
	Boss->SetActorRotation(Data.CRDirection.Rotation());
 
	// 좌우 벡터 계산
	const FVector RightVector = FVector::CrossProduct(FVector::UpVector, Data.CRDirection).GetSafeNormal();
 
	// 왼쪽 분신 스폰 (보스 기준 왼쪽)
	const FVector LeftSpawnLoc = Data.CRStartLocation - RightVector * CR_CloneSpacing;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Boss;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 
	ABossCloneActor* LeftClone = Boss->GetWorld()->SpawnActor<ABossCloneActor>(
		ABossCloneActor::StaticClass(), LeftSpawnLoc,
		Data.CRDirection.Rotation(), SpawnParams);
 
	if (LeftClone)
	{
		const FVector LeftToPlayer = (Player->GetActorLocation() - LeftSpawnLoc).GetSafeNormal2D();
		LeftClone->InitRush(LeftToPlayer, CR_RushSpeed, CR_RushDistance, CR_Damage, CR_RushWidth, CR_KnockbackForce);
 
		// 보스 메시 복사 (외형 동일하게)
		if (USkeletalMeshComponent* BossMesh = Boss->GetMesh())
		{
			if (USkeletalMeshComponent* CloneMesh = LeftClone->FindComponentByClass<USkeletalMeshComponent>())
			{
				CloneMesh->SetSkeletalMesh(BossMesh->GetSkeletalMeshAsset());
			}
		}
 
		Data.LeftClone = LeftClone;
		UE_LOG(LogTemp, Log, TEXT("[CloneRush] 왼쪽 분신 스폰: %s"), *LeftSpawnLoc.ToString());
	}
 
	// 오른쪽 분신 스폰 (보스 기준 오른쪽)
	const FVector RightSpawnLoc = Data.CRStartLocation + RightVector * CR_CloneSpacing;
 
	ABossCloneActor* RightClone = Boss->GetWorld()->SpawnActor<ABossCloneActor>(
		ABossCloneActor::StaticClass(), RightSpawnLoc,
		Data.CRDirection.Rotation(), SpawnParams);
 
	if (RightClone)
	{
		const FVector RightToPlayer = (Player->GetActorLocation() - RightSpawnLoc).GetSafeNormal2D();
		LeftClone->InitRush(RightToPlayer, CR_RushSpeed, CR_RushDistance, CR_Damage, CR_RushWidth, CR_KnockbackForce);
 
		if (USkeletalMeshComponent* BossMesh = Boss->GetMesh())
		{
			if (USkeletalMeshComponent* CloneMesh = RightClone->FindComponentByClass<USkeletalMeshComponent>())
			{
				CloneMesh->SetSkeletalMesh(BossMesh->GetSkeletalMeshAsset());
			}
		}
 
		Data.RightClone = RightClone;
		UE_LOG(LogTemp, Log, TEXT("[CloneRush] 오른쪽 분신 스폰: %s"), *RightSpawnLoc.ToString());
	}
 
	UE_LOG(LogTemp, Log, TEXT("[CloneRush] 3체 준비 완료 - 준비 동작 시작"));
 
	return EStateTreeRunStatus::Running;
}
 

// 분신 돌진 - Tick (왼쪽 → 오른쪽 → 본체 순차 돌진)

EStateTreeRunStatus FSTTask_BossAttackExecutor::TickCloneRush(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
 
	switch (Data.CRPhase)
	{
	case ECRPhase::SpawnClones:
		// 스폰 직후 바로 WindUp으로
		Data.CRPhase = ECRPhase::WindUp;
		Data.PhaseTimer = 0.f;
		break;
 
	case ECRPhase::WindUp:
		// 분신/본신 3체 모두 준비 동작 대기
		if (Data.PhaseTimer >= CR_WindUpDuration)
		{
			// 왼쪽 분신부터 돌진!
			if (ABossCloneActor* LeftClone = Cast<ABossCloneActor>(Data.LeftClone))
			{
				LeftClone->StartRush();
			}
			Data.CRPhase = ECRPhase::LeftRush;
			Data.PhaseTimer = 0.f;
			UE_LOG(LogTemp, Log, TEXT("[CloneRush] 왼쪽 분신 돌진 시작!"));
		}
		break;
 
	case ECRPhase::LeftRush:
		{
			// 왼쪽 분신 돌진 완료 대기
			ABossCloneActor* LeftClone = Cast<ABossCloneActor>(Data.LeftClone);
			if (!LeftClone || LeftClone->IsRushComplete())
			{
				Data.CRPhase = ECRPhase::LeftDelay;
				Data.PhaseTimer = 0.f;
				UE_LOG(LogTemp, Log, TEXT("[CloneRush] 왼쪽 완료 → 딜레이"));
			}
		}
		break;
 
	case ECRPhase::LeftDelay:
		// 순차 딜레이
		if (Data.PhaseTimer >= CR_SequenceDelay)
		{
			// 오른쪽 분신 돌진!
			if (ABossCloneActor* RightClone = Cast<ABossCloneActor>(Data.RightClone))
			{
				RightClone->StartRush();
			}
			Data.CRPhase = ECRPhase::RightRush;
			Data.PhaseTimer = 0.f;
			UE_LOG(LogTemp, Log, TEXT("[CloneRush] 오른쪽 분신 돌진 시작!"));
		}
		break;
 
	case ECRPhase::RightRush:
		{
			// 오른쪽 분신 돌진 완료 대기
			ABossCloneActor* RightClone = Cast<ABossCloneActor>(Data.RightClone);
			if (!RightClone || RightClone->IsRushComplete())
			{
				Data.CRPhase = ECRPhase::RightDelay;
				Data.PhaseTimer = 0.f;
				UE_LOG(LogTemp, Log, TEXT("[CloneRush] 오른쪽 완료 → 본체 준비"));
			}
		}
		break;
 
	case ECRPhase::RightDelay:
		// 본체 돌진 전 딜레이
		if (Data.PhaseTimer >= CR_SequenceDelay)
		{
			Data.CRPhase = ECRPhase::BossRush;
			Data.PhaseTimer = 0.f;
			UE_LOG(LogTemp, Log, TEXT("[CloneRush] 본체 돌진 시작!"));
		}
		break;
 
	case ECRPhase::BossRush:
		{
			// 본체 직접 돌진
			const FVector CurrentLoc = Boss->GetActorLocation();
			const FVector NewLoc = CurrentLoc + Data.CRDirection * CR_RushSpeed * DeltaTime;
			const float DistanceTraveled = FVector::Dist2D(Data.CRStartLocation, NewLoc);
 
			if (DistanceTraveled >= CR_RushDistance)
			{
				Boss->SetActorLocation(Data.CRTargetLocation);
				Data.CRPhase = ECRPhase::Recovery;
				Data.PhaseTimer = 0.f;
				UE_LOG(LogTemp, Log, TEXT("[CloneRush] 본체 돌진 완료 → 회복"));
			}
			else
			{
				Boss->SetActorLocation(NewLoc);
 
				// 본체 돌진 데미지
				if (!Data.bBossRushDamageApplied)
				{
					ApplyDamageInRadius(Boss, Boss->GetActorLocation(), CR_RushWidth,
						CR_Damage, CR_KnockbackForce, Data.CRDirection);
 
					// 데미지 적용 여부는 ApplyDamageInRadius 안에서 확인
					TArray<FOverlapResult> Overlaps;
					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(Boss);
					Boss->GetWorld()->OverlapMultiByChannel(
						Overlaps, Boss->GetActorLocation(), FQuat::Identity,
						ECC_Pawn, FCollisionShape::MakeSphere(CR_RushWidth), QueryParams);
					for (const FOverlapResult& Overlap : Overlaps)
					{
						if (ACharacter* HitChar = Cast<ACharacter>(Overlap.GetActor()))
						{
							if (HitChar != Boss)
							{
								Data.bBossRushDamageApplied = true;
								break;
							}
						}
					}
				}
			}
		}
		break;
 
	case ECRPhase::Recovery:
		if (Data.PhaseTimer >= CR_RecoveryDuration)
		{
			UE_LOG(LogTemp, Log, TEXT("[CloneRush] 공격 완료"));
			return EStateTreeRunStatus::Succeeded;
		}
		break;
 
	default:
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 

// 공통 데미지 적용

void FSTTask_BossAttackExecutor::ApplyDamageInRadius(
	AActor* BossActor, const FVector& Center, float Radius,
	float Damage, float Knockback, const FVector& KnockbackDir) const
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(BossActor);
 
	BossActor->GetWorld()->OverlapMultiByChannel(
		Overlaps, Center, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(Radius),
		QueryParams);
 
	for (const FOverlapResult& Overlap : Overlaps)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(Overlap.GetActor());
		if (!HitCharacter || HitCharacter == BossActor)
		{
			continue;
		}
 
		FDamageEvent DamageEvent;
		HitCharacter->TakeDamage(Damage, DamageEvent, nullptr, BossActor);
 
		UE_LOG(LogTemp, Log, TEXT("[Attack] %s에게 %.0f 데미지!"),
			*HitCharacter->GetName(), Damage);
 
		if (UCharacterMovementComponent* Mov = HitCharacter->GetCharacterMovement())
		{
			const FVector Dir = (HitCharacter->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal();
			Mov->AddImpulse(Dir * Knockback, true);
		}
	}
}
