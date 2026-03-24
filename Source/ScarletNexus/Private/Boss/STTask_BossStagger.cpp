// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossStagger.h"
#include "Boss/STTask_BossHitReaction.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Boss/IBossCharacterInterface.h"
 
EStateTreeRunStatus FSTTask_BossStagger::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime = 0.f;
	InstanceData.bBrainCrushTriggered = false;
	InstanceData.SubPhase = EStaggerSubPhase::EnterStagger;
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	
	// 이동 정지
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
	}
 
	// 현재 재생 중인 몽타주 강제 중단
	UAnimInstance* AnimInstance = BossChar->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.15f);
	}
 
	// 그로기 진입 몽타주 재생
	TransitionToSubPhase(InstanceData, BossChar, EStaggerSubPhase::EnterStagger);
 
	UE_LOG(LogTemp, Log, TEXT("[BossStagger] 그로기 상태 진입 (Phase: %d)"),
		static_cast<int32>(InstanceData.CurrentPhase));
 
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossStagger::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime += DeltaTime;
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	UAnimInstance* AnimInstance = BossChar->GetMesh()->GetAnimInstance();
 
	switch (InstanceData.SubPhase)
	{
	// 진입 모션 재생 중
	case EStaggerSubPhase::EnterStagger:
	{
		// 진입 몽타주가 끝나면 루프로 전환
		if (!AnimInstance || !AnimInstance->IsAnyMontagePlaying())
		{
			TransitionToSubPhase(InstanceData, BossChar, EStaggerSubPhase::StaggerLoop);
		}
		break;
	}
 
	// 그로기 루프 (플레이어의 브레인 크래시 입력 대기)
	case EStaggerSubPhase::StaggerLoop:
	{
		// 브레인 크래시 발동 체크
		// 실제로는 플레이어 입력 시스템에서 이벤트를 받아야 함
		// 여기서는 bBrainCrushAvailable이 외부에서 true로 설정된 경우 발동
		if (InstanceData.bBrainCrushTriggered)
		{
			TransitionToSubPhase(InstanceData, BossChar, EStaggerSubPhase::BrainCrush);
			break;
		}
 
		// 시간 초과 시 자동 해제
		if (InstanceData.ElapsedTime >= MaxStaggerDuration)
		{
			UE_LOG(LogTemp, Log, TEXT("[BossStagger] 그로기 시간 초과 -> 자동 해제"));
			TransitionToSubPhase(InstanceData, BossChar, EStaggerSubPhase::RecoverFromStagger);
		}
 
		// 브레인 크래시 입력 윈도우 초과 시에도 해제
		if (InstanceData.ElapsedTime >= BrainCrushWindowDuration && !InstanceData.bBrainCrushTriggered)
		{
			UE_LOG(LogTemp, Log, TEXT("[BossStagger] 브레인 크래시 윈도우 만료"));
			TransitionToSubPhase(InstanceData, BossChar, EStaggerSubPhase::RecoverFromStagger);
		}
		break;
	}
 
	
	// 브레인 크래시 연출 중
	case EStaggerSubPhase::BrainCrush:
	{
		// 브레인 크래시 몽타주가 끝나면 완료
		if (!AnimInstance || !AnimInstance->IsAnyMontagePlaying())
		{
			// 보너스 대미지 적용
			if (BossChar->GetClass()->ImplementsInterface(UBossCharacterInterface::StaticClass()))
			{
				const float CurrentHP = IBossCharacterInterface::Execute_GetCurrentHP(BossChar);
				// BrainCrushDamageRatio 만큼 MaxHP 비례 대미지
				// 실제 MaxHP는 BossConfig에서 가져와야 하지만, 간단하게 처리
				const float BonusDamage = 10000.f * BrainCrushDamageRatio;
				IBossCharacterInterface::Execute_ApplyDamage(BossChar, BonusDamage, nullptr);
 
				UE_LOG(LogTemp, Log,
					TEXT("[BossStagger] 브레인 크래시 대미지: %.0f"), BonusDamage);
			}
 
			return EStateTreeRunStatus::Succeeded;
		}
		break;
	}
 
	// 그로기 해제 모션 재생 중
	case EStaggerSubPhase::RecoverFromStagger:
	{
		if (!AnimInstance || !AnimInstance->IsAnyMontagePlaying())
		{
			return EStateTreeRunStatus::Succeeded;
		}
		break;
	}
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_BossStagger::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	ACharacter* BossChar = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossChar)
	{
		return;
	}
 
	// 경직 게이지 리셋
	if (BossChar->GetClass()->ImplementsInterface(UBossCharacterInterface::StaticClass()))
	{
		// 경직 게이지를 0으로 리셋
		// ApplyStaggerDamage에 음수를 넣는 대신, 별도 리셋 함수가 이상적
		// 지금은 캐릭터 클래스에서 직접 리셋하도록 위임
	}
 
	// 이동 재활성화
	if (UCharacterMovementComponent* MoveComp = BossChar->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossStagger] 그로기 상태 종료 (BrainCrush: %s, 경과: %.1fs)"),
		InstanceData.bBrainCrushTriggered ? TEXT("Yes") : TEXT("No"),
		InstanceData.ElapsedTime);
}
 
void FSTTask_BossStagger::TransitionToSubPhase(
	FInstanceDataType& InstanceData,
	ACharacter* BossChar,
	EStaggerSubPhase NewSubPhase) const
{
	InstanceData.SubPhase = NewSubPhase;
 
	UAnimInstance* AnimInstance = BossChar ? BossChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}
 
	UAnimMontage* MontageToPlay = nullptr;
 
	switch (NewSubPhase)
	{
	case EStaggerSubPhase::EnterStagger:
		MontageToPlay = StaggerEnterMontage;
		break;
 
	case EStaggerSubPhase::StaggerLoop:
		MontageToPlay = StaggerLoopMontage;
		break;
 
	case EStaggerSubPhase::BrainCrush:
		MontageToPlay = BrainCrushReceiveMontage;
		// TODO: 카메라 연출 시작
		// TODO: 시간 슬로우 효과
		// TODO: 플레이어 브레인 크래시 공격 몽타주 트리거
		UE_LOG(LogTemp, Log, TEXT("[BossStagger] 브레인 크래시 발동!"));
		break;
 
	case EStaggerSubPhase::RecoverFromStagger:
		MontageToPlay = StaggerRecoverMontage;
		break;
	}
 
	if (MontageToPlay)
	{
		AnimInstance->Montage_Stop(0.15f);
		AnimInstance->Montage_Play(MontageToPlay, 1.f);
	}
}