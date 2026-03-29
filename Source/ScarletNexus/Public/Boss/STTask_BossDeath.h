// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_BossDeath.generated.h"
 
USTRUCT()
struct FSTTask_BossDeathInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	float Timer = 0.f;
	bool bDeathStarted = false;
 
	// 사망 시 시작 위치 (자동 저장)
	FVector StartLocation = FVector::ZeroVector;
};
 
USTRUCT(meta = (DisplayName = "Boss Death"))
struct SCARLETNEXUS_API FSTTask_BossDeath : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossDeathInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
 
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context, float DeltaTime) const override;
 
	// 가라앉는 데 걸리는 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Death")
	float DeathDuration = 5.0f;
 
	// 가라앉은 후 숨기기까지 대기 시간
	UPROPERTY(EditAnywhere, Category = "Death")
	float HideDelay = 1.0f;
 
	// 가라앉는 깊이 (cm)
	UPROPERTY(EditAnywhere, Category = "Death")
	float SinkDepth = 300.f;
	
	// 사망 후 멈춰있는 시간
	UPROPERTY(EditAnywhere, Category = "Death")
	float PauseBeforeSink = 1.5f;
};