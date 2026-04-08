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
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float DistanceToPlayer = 0.f;
 
	// --- 내부 상태 ---
	UPROPERTY()
	float ElapsedTime = 0.f;
 
	UPROPERTY()
	float WaitDuration = 0.f;
 
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	// --- 회피 텔레포트 상태 ---
	UPROPERTY()
	bool bIsEvadeTeleporting = false;
 
	UPROPERTY()
	float EvadeTeleportTimer = 0.f;
 
	UPROPERTY()
	FVector EvadeTeleportTarget = FVector::ZeroVector;
 
	UPROPERTY()
	float EvadeCooldownTimer = 0.f;
	
 
	// --- 순찰 텔레포트 상태 ---
	UPROPERTY()
	bool bIsPatrolTeleporting = false;
 
	UPROPERTY()
	float PatrolTeleportTimer = 0.f;
 
	UPROPERTY()
	FVector PatrolTeleportTarget = FVector::ZeroVector;
 
	UPROPERTY()
	float PatrolCooldownTimer = 0.f;
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
 
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;
 
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;
 
	// --- 에디터 설정 ---
	UPROPERTY(EditAnywhere, Category = "Config")
	float WalkSpeed = 150.f;
 
	UPROPERTY(EditAnywhere, Category = "Config")
	float PreferredDistance = 500.f;
 
	UPROPERTY(EditAnywhere, Category = "Config")
	float DistanceTolerance = 100.f;
 
	UPROPERTY(EditAnywhere, Category = "Config")
	FVector2D WaitTimeRange = FVector2D(1.5f, 3.f);
 
	UPROPERTY(EditAnywhere, Category = "Config")
	float RotationSpeed = 360.f;
 
	// --- 회피 텔레포트 설정 ---
	UPROPERTY(EditAnywhere, Category = "EvadeTeleport")
	float EvadeTriggerDistance = 200.f;
 
	UPROPERTY(EditAnywhere, Category = "EvadeTeleport")
	float EvadeTeleportDistance = 600.f;
 
	UPROPERTY(EditAnywhere, Category = "EvadeTeleport")
	float EvadeVanishDuration = 0.2f;
 
	UPROPERTY(EditAnywhere, Category = "EvadeTeleport")
	float EvadeCooldown = 5.f;
 
	// --- 순찰 텔레포트 설정 ---
	UPROPERTY(EditAnywhere, Category = "PatrolTeleport")
	float PatrolMinDistance = 300.f;
 
	UPROPERTY(EditAnywhere, Category = "PatrolTeleport")
	float PatrolMaxDistance = 600.f;
 
	UPROPERTY(EditAnywhere, Category = "PatrolTeleport")
	float PatrolVanishDuration = 0.15f;
 
	UPROPERTY(EditAnywhere, Category = "PatrolTeleport")
	FVector2D PatrolIntervalRange = FVector2D(1.0f, 2.0f);
};