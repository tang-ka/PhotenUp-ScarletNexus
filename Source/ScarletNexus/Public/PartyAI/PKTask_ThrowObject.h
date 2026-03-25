// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PKTask_ThrowObject.generated.h"

class APlayerCharacterBase;
class APKObject;

USTRUCT()
struct FPKTask_ThrowObjectInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<APKObject> ThrownObject = nullptr;
	
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<APlayerCharacterBase> PartyTarget = nullptr;
	
	UPROPERTY(EditAnywhere)
	float ThrowSpeed = 2000.f;
	
	UPROPERTY(EditAnywhere)
	float ImpactDamage = 45.f;
};

USTRUCT(DisplayName="PK: Throw Object")
struct FPKTask_ThrowObject  : public FStateTreeTaskBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPKTask_ThrowObjectInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};