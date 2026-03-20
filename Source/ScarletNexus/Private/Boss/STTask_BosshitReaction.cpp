// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossHitReaction.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
 
EStateTreeRunStatus FSTTask_BossHitReaction::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bReactionFinished = false;
	InstanceData.AppliedReactionType = ReactionType;
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 피격 방향 계산
	// 실제로는 대미지 이벤트에서 Causer를 받아야 하지만,
	// 여기서는 플레이어를 Causer로 간주
	const ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(
		BossChar->GetWorld(), 0);
 
	if (PlayerChar)
	{
		InstanceData.HitDirection = CalculateHitDirection(BossChar, PlayerChar);
	}
	else
	{
		InstanceData.HitDirection = EHitDirection::Front;
	}
 
	// 리액션 몽타주 재생
	UAnimMontage* Montage = GetMontageForDirection(InstanceData.HitDirection);
	if (!Montage)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BossHitReaction] 몽타주가 없습니다. ReactionType: %d, Direction: %d"),
			static_cast<int32>(ReactionType),
			static_cast<int32>(InstanceData.HitDirection));
		// 몽타주 없어도 짧은 경직은 적용
		if (ReactionType == EHitReactionType::Flinch)
		{
			return EStateTreeRunStatus::Succeeded;
		}
		return EStateTreeRunStatus::Failed;
	}
 
	UAnimInstance* AnimInstance = BossChar->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 현재 재생 중인 몽타주 중단 후 피격 몽타주 재생
	AnimInstance->Montage_Stop(0.1f);
	AnimInstance->Montage_Play(Montage, 1.f);
 
	// 넉백 / 에어본 물리 적용
	if (ReactionType == EHitReactionType::Knockback && PlayerChar)
	{
		const FVector KnockbackDir =
			(BossChar->GetActorLocation() - PlayerChar->GetActorLocation()).GetSafeNormal();
 
		BossChar->LaunchCharacter(
			KnockbackDir * KnockbackForce,
			true,  // XY Override
			false  // Z Override
		);
 
		UE_LOG(LogTemp, Log, TEXT("[BossHitReaction] 넉백 적용: Force=%.0f, Dir=%s"),
			KnockbackForce, *KnockbackDir.ToString());
	}
	else if (ReactionType == EHitReactionType::Airborne)
	{
		BossChar->LaunchCharacter(
			FVector(0.f, 0.f, LaunchHeight),
			false, // XY Override
			true   // Z Override
		);
 
		UE_LOG(LogTemp, Log, TEXT("[BossHitReaction] 에어본 적용: Height=%.0f"), LaunchHeight);
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossHitReaction] 피격 리액션 시작: Type=%d, Dir=%d"),
		static_cast<int32>(ReactionType),
		static_cast<int32>(InstanceData.HitDirection));
 
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossHitReaction::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	UAnimInstance* AnimInstance = BossChar->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 몽타주가 끝나면 리액션 완료
	if (!AnimInstance->IsAnyMontagePlaying())
	{
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_BossHitReaction::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	// 리액션이 끝나면 별도 정리 작업 없음
	// 상위 State에서 Idle로 돌아가며 자연스럽게 다음 행동 선택
}
 
EHitDirection FSTTask_BossHitReaction::CalculateHitDirection(
	const AActor* BossActor,
	const AActor* DamageCauser) const
{
	const FVector ToAttacker =
		(DamageCauser->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal();
	const FVector Forward = BossActor->GetActorForwardVector();
	const FVector Right = BossActor->GetActorRightVector();
 
	const float ForwardDot = FVector::DotProduct(Forward, ToAttacker);
	const float RightDot = FVector::DotProduct(Right, ToAttacker);
 
	// 전방/후방 우선 판정, 그 다음 좌우
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return (ForwardDot > 0.f) ? EHitDirection::Front : EHitDirection::Back;
	}
	else
	{
		return (RightDot > 0.f) ? EHitDirection::Right : EHitDirection::Left;
	}
}
 
UAnimMontage* FSTTask_BossHitReaction::GetMontageForDirection(EHitDirection Direction) const
{
	const int32 DirectionIndex = static_cast<int32>(Direction);
 
	if (DirectionalMontages.IsValidIndex(DirectionIndex))
	{
		return DirectionalMontages[DirectionIndex];
	}
 
	// 방향별 몽타주가 없으면 범용 몽타주 사용
	return FallbackMontage;
}