// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PKTask_FindObject.generated.h"

class APKObject;

USTRUCT()
struct FPKTask_FindObjectInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float SearchRadius = 800.f;
	
	UPROPERTY(EditAnywhere, meta=(Output))
	TObjectPtr<APKObject> FoundObject = nullptr;
};

USTRUCT(DisplayName = "PK: Find Object")
struct FPKTask_FindObject : public FStateTreeTaskBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPKTask_FindObjectInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
