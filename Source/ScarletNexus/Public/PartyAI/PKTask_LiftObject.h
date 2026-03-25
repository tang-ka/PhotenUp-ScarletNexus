// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PKTask_LiftObject.generated.h"

class APKObject;

USTRUCT()
struct FPKTask_LiftObjectInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<APKObject> TargetPKObject = nullptr;
	
	UPROPERTY(EditAnywhere)
	float LiftHeight = 120.f;
	
	UPROPERTY(EditAnywhere)
	float LiftInterpSpeed = 6.f;
	
	UPROPERTY(EditAnywhere)
	float AimReadyTime = 0.8f;
	
	float ElapsedTime = 0.f;
};

USTRUCT(DisplayName="PK: Lift Object")
struct FPKTask_LiftObject : public FStateTreeTaskBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPKTask_LiftObjectInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};