// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/ActionManagerComponent.h"

#include "ScarletNexus.h"


UActionManagerComponent::UActionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActionManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ForceSetState(EActionState::Idle);
}

bool UActionManagerComponent::CanAttack() const
{
	switch (CurState)
	{
	case EActionState::Idle:
		return true;

	case EActionState::Attacking:
		return bComboWindowOpen;

	case EActionState::Dashing:
		// Dashing 완료 후 Idle로 전이된 뒤 공격 가능
		// Dashing 상태 자체에서는 불가, 
		// 대시 완료 시 외부에서 ForceSetState(Idle) 호출 필요
		return false;

	case EActionState::Jumping:
		return true;  // 공중 공격 허용

	case EActionState::Staggered:
		return false;
	case EActionState::Dead:
		return false;

	default:
		return false;
	}
}

bool UActionManagerComponent::CanMove() const
{
	 // Attack, Staggered, Dead 상태에서는 이동 불가
	return CurState != EActionState::Attacking &&
		   CurState != EActionState::Staggered &&
		   CurState != EActionState::Dead;
}

bool UActionManagerComponent::CanDash() const
{
	return true;
}

bool UActionManagerComponent::CanJump() const
{
	return true;
}

bool UActionManagerComponent::TrySetState(EActionState NewState)
{	
	// 유효한 전이인지 체크
	if (!IsValidTransition(CurState, NewState))
	{
		PRINTLOG_SH(TEXT("[ActionManager] 상태 전이 거부: %s -> %s"),
					*GetStateName(CurState),
					*GetStateName(NewState));
		return false;
	}

	ForceSetState(NewState);
	return true;
}

void UActionManagerComponent::ForceSetState(EActionState NewState)
{
	EActionState OldState = CurState;
	CurState = NewState;

	CloseComboWindow();

	PRINTLOG_SH(TEXT("[ActionManager] 상태 전이: %s -> %s"),
				*GetStateName(OldState),
				*GetStateName(NewState));

	OnActionStateChanged.Broadcast(OldState, NewState);
}

void UActionManagerComponent::OpenComboWindow()
{
	if (CurState != EActionState::Attacking)
	{
		PRINTLOG_SH(TEXT("[ActionManager] 콤보 윈도우 열기 실패 — 현재 상태: %s"),
					*GetStateName(CurState));
		return;
	}

	bComboWindowOpen = true;
	PRINTLOG_SH(TEXT("[ActionManager] 콤보 윈도우 열림"));
}

void UActionManagerComponent::CloseComboWindow()
{
	bComboWindowOpen = false;
	PRINTLOG_SH(TEXT("[ActionManager] 콤보 윈도우 닫힘"));
}

bool UActionManagerComponent::IsValidTransition(EActionState From, EActionState To) const
{
	// Dead 상태에서는 어디로도 전이 불가
	if (From == EActionState::Dead)
	{
		return false;
	}

	// Staggered 상태에서는 Idle로만 복귀 가능
	if (From == EActionState::Staggered)
	{
		return To == EActionState::Idle;
	}

	// 어디서든 Dead, Staggered로는 전이 가능
	if (To == EActionState::Dead || To == EActionState::Staggered)
	{
		return true;
	}

	switch (From)
	{
	case EActionState::Idle:
		// Idle에서는 모든 행동 상태로 전이 가능
		return true;

	case EActionState::Attacking:
		if (To == EActionState::Attacking)
		{
			// Attacking -> Attacking은 콤보 윈도우가 열려 있을 때만 허용
			return bComboWindowOpen;
		}
		// Attacking -> Idle (공격 종료)
		return To == EActionState::Idle;

	case EActionState::Dashing:
		// Dashing -> Idle (대시 완료)
		// Dashing -> Attacking (대시 후 즉시 공격)
		return To == EActionState::Idle || To == EActionState::Attacking;

	case EActionState::Jumping:
		// Jumping -> Idle (착지)
		// Jumping -> Attacking (공중 공격)
		return To == EActionState::Idle || To == EActionState::Attacking;

	default:
		return false;
	}
}

FString UActionManagerComponent::GetStateName(EActionState State) const
{
	const UEnum* EnumPtr = StaticEnum<EActionState>();
	if (EnumPtr)
	{
		return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(State)).ToString();
	}
	return FString::Printf(TEXT("%d"), static_cast<int32>(State));
}

