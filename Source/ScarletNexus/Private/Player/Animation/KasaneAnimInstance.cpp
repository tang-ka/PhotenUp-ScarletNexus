// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animation/KasaneAnimInstance.h"

#include "Player/PlayerKasane.h"
#include "Player/Component/ActionManagerComponent.h"
#include "Player/Component/BladeHandlerComponent.h"

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
	}
}
