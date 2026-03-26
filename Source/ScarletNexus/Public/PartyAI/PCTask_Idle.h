// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PCTask_Idle.generated.h"

class APlayerCharacterBase;

USTRUCT()
struct FPCTask_FollowInstanceData
{
	GENERATED_BODY()
	
	// Evaluator에서 받아온 플레이어
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<APlayerCharacterBase> FollowTarget = nullptr;
	
	// 이 거리 이내면 정지
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 200.f;
	
	// 이 거리 밖이면 이동 시작
	UPROPERTY(EditAnywhere)
	float FollowStartDistance = 400.f;
	
	bool bIsMoving = false;
};

USTRUCT(DisplayName="Party: Follow Player (Idle)")
struct FPCTask_Follow : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPCTask_FollowInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};