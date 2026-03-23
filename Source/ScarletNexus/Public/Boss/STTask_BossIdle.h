// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossIdle.generated.h"
 

// Task가 기억하는 데이터

USTRUCT()
struct FSTTask_BossIdleInstanceData
{
	GENERATED_BODY()
 
	// --- 파라미터 (바인딩 선택사항) ---
 
	// 플레이어까지 거리
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DistanceToPlayer = 0.f;
 
	// --- 내부 상태 ---
 
	// 대기 시간 경과
	UPROPERTY()
	float ElapsedTime = 0.f;
 
	// 이번 대기에서 기다릴 총 시간 (랜덤으로 정해짐)
	UPROPERTY()
	float WaitDuration = 0.f;
 
	// Context Actor (보스 캐릭터)
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
};
 

// Boss Idle Task
USTRUCT(DisplayName = "Boss idle walk")
struct SCARLETNEXUS_API FSTTask_BossIdle : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossIdleInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	// 상태 진입: 대기 시간 랜덤 결정
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// 매 프레임: 플레이어를 바라보며 천천히 이동
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;
 
	// 상태 퇴장: 이동 정지
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// --- 에디터 설정 ---
 
	// 걷기 속도 (기본 이동 속도보다 느리게)
	UPROPERTY(EditAnywhere, Category = "Config")
	float WalkSpeed = 150.f;
 
	// 플레이어와 유지할 적정 거리
	UPROPERTY(EditAnywhere, Category = "Config")
	float PreferredDistance = 500.f;
 
	// 적정 거리 허용 오차 (이 범위 안이면 이동 안 함)
	UPROPERTY(EditAnywhere, Category = "Config")
	float DistanceTolerance = 100.f;
 
	// 대기 시간 범위 (이 사이에서 랜덤)
	UPROPERTY(EditAnywhere, Category = "Config")
	FVector2D WaitTimeRange = FVector2D(1.5f, 3.f);
 
	// 플레이어를 향한 회전 속도 (도/초)
	UPROPERTY(EditAnywhere, Category = "Config")
	float RotationSpeed = 360.f;
};