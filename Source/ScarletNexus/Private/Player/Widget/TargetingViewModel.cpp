// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget/TargetingViewModel.h"

#include "Player/Component/PlayerPerceptionComponent.h"

void UTargetingViewModel::Initialize(UPlayerPerceptionComponent* InPerceptionComp)
{
	if (!IsValid(InPerceptionComp))
	{
		return;
	}

	// 이미 구독 중이라면 먼저 해제
	Shutdown();

	PerceptionComp = InPerceptionComp;

	HardTargetHandle = InPerceptionComp->OnHardTargetChanged.AddUObject(
		this, &UTargetingViewModel::HandleHardTargetChanged);

	PKTargetHandle = InPerceptionComp->OnPsychokinesisTargetChanged.AddUObject(
		this, &UTargetingViewModel::HandlePKTargetChanged);
}

void UTargetingViewModel::Shutdown()
{
	if (UPlayerPerceptionComponent* Comp = PerceptionComp.Get())
	{
		Comp->OnHardTargetChanged.Remove(HardTargetHandle);
		Comp->OnPsychokinesisTargetChanged.Remove(PKTargetHandle);
	}

	HardTargetHandle.Reset();
	PKTargetHandle.Reset();
	PerceptionComp.Reset();
	HardTarget.Reset();
	PKTarget.Reset();
}

void UTargetingViewModel::HandleHardTargetChanged(AActor* NewTarget)
{
	HardTarget = NewTarget;
	OnHardTargetUpdated.Broadcast(NewTarget);
}

void UTargetingViewModel::HandlePKTargetChanged(AActor* NewTarget)
{
	PKTarget = NewTarget;
	OnPKTargetUpdated.Broadcast(NewTarget);
}
