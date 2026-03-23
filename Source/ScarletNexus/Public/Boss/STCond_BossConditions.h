// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "BossTypes.h"
#include "StateTreeExecutionContext.h"
#include "STCond_BossConditions.generated.h"



// 페이즈 체크 Condition
USTRUCT()
struct FSTCond_IsPhaseInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
};
 
USTRUCT(DisplayName = "Is boss phase")
struct SCARLETNEXUS_API FSTCond_IsPhase : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTCond_IsPhaseInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override
	{
		const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		return InstanceData.CurrentPhase == RequiredPhase;
	}
 
	// 확인할 페이즈
	UPROPERTY(EditAnywhere, Category = "Config")
	EBossPhase RequiredPhase = EBossPhase::Phase1;
};
 

// 거리 체크 Condition
USTRUCT()
struct FSTCond_DistanceCheckInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	float DistanceToPlayer = 0.f;
};
 
USTRUCT(DisplayName = "Boss distance check")
struct SCARLETNEXUS_API FSTCond_DistanceCheck : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTCond_DistanceCheckInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override
	{
		const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
		if (bCheckMinDistance && InstanceData.DistanceToPlayer < MinDistance)
		{
			return false;
		}
		if (bCheckMaxDistance && InstanceData.DistanceToPlayer > MaxDistance)
		{
			return false;
		}
		return true;
	}
 
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bCheckMinDistance = false;
 
	UPROPERTY(EditAnywhere, Category = "Config", meta = (EditCondition = "bCheckMinDistance"))
	float MinDistance = 0.f;
 
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bCheckMaxDistance = true;
 
	UPROPERTY(EditAnywhere, Category = "Config", meta = (EditCondition = "bCheckMaxDistance"))
	float MaxDistance = 300.f;
};
 
// 경직/브레인 크래시 체크 Condition

USTRUCT()
struct FSTCond_StaggerCheckInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	float StaggerRatio = 0.f;
 
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bBrainCrushAvailable = false;
};
 
USTRUCT(DisplayName = "Boss stagger check")
struct SCARLETNEXUS_API FSTCond_StaggerCheck : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTCond_StaggerCheckInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override
	{
		const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
		if (bCheckBrainCrush)
		{
			return InstanceData.bBrainCrushAvailable;
		}
 
		return InstanceData.StaggerRatio >= StaggerThreshold;
	}
 
	// 브레인 크래시 가능 여부만 체크
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bCheckBrainCrush = false;
 
	// 경직 게이지 임계값 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, Category = "Config", meta = (EditCondition = "!bCheckBrainCrush", ClampMin = "0.0", ClampMax = "1.0"))
	float StaggerThreshold = 0.5f;
};
 

// 페이즈 전환 중 체크 Condition

USTRUCT()
struct FSTCond_PhaseTransitionInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsPhaseTransitioning = false;
};
 
USTRUCT(DisplayName = "Boss phase transitioning")
struct SCARLETNEXUS_API FSTCond_PhaseTransitioning : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTCond_PhaseTransitionInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override
	{
		const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		return InstanceData.bIsPhaseTransitioning == bExpectedValue;
	}
 
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bExpectedValue = true;
};
 

// 쿨다운 타이머 Condition

USTRUCT()
struct FSTCond_AttackCooldownInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	float TimeSinceLastAttack = 0.f;
};
 
USTRUCT(DisplayName = "Boss attack cooldown ready")
struct SCARLETNEXUS_API FSTCond_AttackCooldownReady : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTCond_AttackCooldownInstanceData;
 
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
 
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override
	{
		const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		return InstanceData.TimeSinceLastAttack >= MinCooldown;
	}
 
	UPROPERTY(EditAnywhere, Category = "Config")
	float MinCooldown = 1.5f;
};