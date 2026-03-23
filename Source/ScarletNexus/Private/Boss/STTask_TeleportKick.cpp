// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_TeleportKick.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "NavigationSystem.h"
#include "Engine/OverlapResult.h"
 
EStateTreeRunStatus FSTTask_TeleportKick::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	Data.AttackPhase = ETeleportKickPhase::Vanishing;
	Data.PhaseTimer = 0.f;
	Data.bDamageApplied = false;
 
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss)
	{
		UE_LOG(LogTemp, Error, TEXT("[TeleportKick] ContextActor가 없습니다."));
		return EStateTreeRunStatus::Failed;
	}
 
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("[TeleportKick] 플레이어를 찾을 수 없습니다."));
		return EStateTreeRunStatus::Failed;
	}
 
	// 현재 위치 저장 (안전장치용)
	Data.OriginalLocation = Boss->GetActorLocation();
 
	// 텔레포트 목표 위치 계산 (플레이어 뒤쪽)
	Data.TeleportTarget = CalculateTeleportTarget(Boss, Player);
 
	UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 공격 시작 - 목표: %s"),
		*Data.TeleportTarget.ToString());
 
	// Vanishing 단계: 보스를 투명하게 (나중에 머티리얼 파라미터로 페이드)
	// 지금은 바로 숨김
	Boss->SetActorHiddenInGame(true);
	Boss->SetActorEnableCollision(false);
 
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_TeleportKick::Tick(
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
	case ETeleportKickPhase::Vanishing:
		{
			// 사라지는 연출 시간 대기
			if (Data.PhaseTimer >= VanishDuration)
			{
				// 텔레포트 실행!
				Boss->SetActorLocation(Data.TeleportTarget);
 
				// 플레이어를 향해 회전
				if (const ACharacter* Player =
					UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0))
				{
					const FVector Dir = (Player->GetActorLocation() - Data.TeleportTarget).GetSafeNormal();
					if (!Dir.IsNearlyZero())
					{
						Boss->SetActorRotation(Dir.Rotation());
					}
				}
 
				Data.AttackPhase = ETeleportKickPhase::Teleporting;
				Data.PhaseTimer = 0.f;
 
				UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 텔레포트 완료 → 등장 대기"));
			}
		}
		break;
 
	case ETeleportKickPhase::Teleporting:
		{
			// 잠깐 대기 후 등장
			if (Data.PhaseTimer >= AppearDelay)
			{
				// 보스 다시 보이기
				Boss->SetActorHiddenInGame(false);
				Boss->SetActorEnableCollision(true);
 
				Data.AttackPhase = ETeleportKickPhase::Kicking;
				Data.PhaseTimer = 0.f;
 
				UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 등장! 킥 공격 시작"));
			}
		}
		break;
 
	case ETeleportKickPhase::Kicking:
		{
			// 킥 데미지 적용 (한번만)
			if (!Data.bDamageApplied)
			{
				ApplyKickDamage(Boss, Boss->GetActorLocation());
				Data.bDamageApplied = true;
			}
 
			// 킥 지속 시간 후 완료
			if (Data.PhaseTimer >= KickDuration)
			{
				Data.AttackPhase = ETeleportKickPhase::Done;
				UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 킥 완료"));
				return EStateTreeRunStatus::Succeeded;
			}
		}
		break;
 
	case ETeleportKickPhase::Done:
		return EStateTreeRunStatus::Succeeded;
 
	default:
		break;
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_TeleportKick::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
 
	// 안전장치: 공격 도중 중단되면 보스를 다시 보이게
	if (AActor* Boss = Data.ContextActor)
	{
		Boss->SetActorHiddenInGame(false);
		Boss->SetActorEnableCollision(true);
	}
}
 
FVector FSTTask_TeleportKick::CalculateTeleportTarget(
	const AActor* BossActor,
	const AActor* PlayerActor) const
{
	const FVector PlayerLoc = PlayerActor->GetActorLocation();
	const FVector PlayerForward = PlayerActor->GetActorForwardVector();
 
	// 플레이어 뒤쪽으로 이동
	FVector Target = PlayerLoc - PlayerForward * TeleportOffset;
 
	// NavMesh 위의 유효 위치로 보정
	if (const UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(BossActor->GetWorld()))
	{
		FNavLocation NavLoc;
		if (NavSys->ProjectPointToNavigation(Target, NavLoc, FVector(300.f, 300.f, 300.f)))
		{
			Target = NavLoc.Location;
		}
	}
 
	return Target;
}
 
void FSTTask_TeleportKick::ApplyKickDamage(
	AActor* BossActor,
	const FVector& KickLocation) const
{
	// 보스 전방 킥 범위 내 플레이어 찾기
	const FVector KickCenter = KickLocation + BossActor->GetActorForwardVector() * (KickRadius * 0.5f);
 
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(BossActor);
 
	BossActor->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		KickCenter,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(KickRadius),
		QueryParams
	);
 
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor || HitActor == BossActor)
		{
			continue;
		}
 
		// ACharacter만 데미지 대상 (Floor 등 제외)
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if (!HitCharacter)
		{
			continue;
		}
 
		// 플레이어에게 데미지 적용
		FDamageEvent DamageEvent;
		HitCharacter->TakeDamage(KickDamage, DamageEvent, nullptr, BossActor);
 
		UE_LOG(LogTemp, Log, TEXT("[TeleportKick] %s에게 %.0f 데미지!"),
			*HitCharacter->GetName(), KickDamage);
 
		// 넉백 적용
		if (UCharacterMovementComponent* Movement = HitCharacter->GetCharacterMovement())
		{
			const FVector KnockbackDir =
				(HitCharacter->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal();
			Movement->AddImpulse(KnockbackDir * KnockbackForce, true);
		}
	}
}