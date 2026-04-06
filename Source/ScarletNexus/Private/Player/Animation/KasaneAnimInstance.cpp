// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animation/KasaneAnimInstance.h"

#include "ScarletNexus.h"
#include "Player/PlayerKasane.h"
#include "Player/Component/ActionManagerComponent.h"
#include "Player/Component/BladeHandlerComponent.h"
#include "Player/Component/ComboComponent.h"

void UKasaneAnimInstance::SetAttackState(EAttackState InState)
{
	PRINTLOG_SH(TEXT("[AnimInstance] 공격 상태 변경: %d -> %d"), AttackState, InState);
	AttackState = InState;
	
	if (AttackState != EAttackState::None)
	{
		SetIsBasicAttacking(true);
		BlendWeight = 1.f; // 공격 애니메이션이 시작되면 BlendWeight를 1로 설정하여 공격 애니메이션이 우선적으로 재생되도록 합니다.
	}
	else
	{
		SetIsBasicAttacking(false);
		BlendWeight = 0.f; // 공격 애니메이션이 끝나면 BlendWeight를 0으로 설정하여 기본 애니메이션으로 전환합니다.
	}
}

void UKasaneAnimInstance::AnimNotify_JumpStart()
{
	bIsJumpEnd = false;
}

void UKasaneAnimInstance::AnimNotify_JumpEnd()
{
	bIsJumpEnd = true;
}

void UKasaneAnimInstance::AnimNotify_CriticalHit()
{
	auto* Player = Cast<APlayerKasane>(TryGetPawnOwner());
	if (Player)
	{
		Player->GetBladeHandlerComp()->SetCanCriticalAllBlades(true); // Critical ON
	}
}

void UKasaneAnimInstance::AnimNotify_AllowMove()
{
	auto* Player = Cast<APlayerKasane>(TryGetPawnOwner());
	if (Player)
	{
		Player->GetActionManagerComp()->SetMovementLocked(false);
		Player->GetActionManagerComp()->ForceSetState(EActionState::Idle);
		Player->GetComboComp()->ResetCombo();
		SetAttackState(EAttackState::None);
	}
}
