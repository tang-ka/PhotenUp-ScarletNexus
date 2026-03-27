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
 
	// 사망 시간 (몽타주 없어서 일단 설정함 )
	UPROPERTY(EditAnywhere, Category = "Death")
	float DeathDuration = 3.0f;
 
	// 사망 후 액터 숨김까지 대기 시간
	UPROPERTY(EditAnywhere, Category = "Death")
	float HideDelay = 2.0f;
};