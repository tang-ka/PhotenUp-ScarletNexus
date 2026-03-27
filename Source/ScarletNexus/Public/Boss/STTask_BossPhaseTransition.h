// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_BossPhaseTransition.generated.h"
 
USTRUCT()
struct FSTTask_BossPhaseTransitionInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	float Timer = 0.f;
	bool bTransitionStarted = false;
};
 
USTRUCT(meta = (DisplayName = "Boss Phase Transition"))
struct SCARLETNEXUS_API FSTTask_BossPhaseTransition : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossPhaseTransitionInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
 
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context, float DeltaTime) const override;
 
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// 페이즈 전환 연출 시간
	UPROPERTY(EditAnywhere, Category = "PhaseTransition")
	float TransitionDuration = 1.5f;
};
