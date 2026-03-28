// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "PCTask_PK.generated.h"

class APKObject;
class APlayerCharacterBase;

UENUM()
enum class EPKPhase : uint8
{
	Find,
	Lift,
	Throw
};

USTRUCT()
struct FPCTask_PKInstanceData
{
	GENERATED_BODY()

	/*UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<APlayerCharacterBase> TargetPlayer = nullptr;*/
	UPROPERTY(EditAnywhere, meta=(Input))
	TObjectPtr<AActor> TargetEnemy = nullptr;
	
	UPROPERTY(EditAnywhere)
	float SearchRadius = 800.f;

	UPROPERTY(EditAnywhere)
	float LiftHeight = 120.f;

	UPROPERTY(EditAnywhere)
	float LiftInterpSpeed = 6.f;

	UPROPERTY(EditAnywhere)
	float AimReadyTime = 0.8f;

	UPROPERTY(EditAnywhere)
	float ThrowSpeed = 2000.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<APKObject> FoundObject = nullptr;
	
	UPROPERTY()
	EPKPhase Phase = EPKPhase::Find;

	// 재생할 몽타주 - ST 에디터에서 캐릭터별로 할당
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> FindMontage = nullptr;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> LiftMontage = nullptr;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> ThrowMontage = nullptr;
	
	UPROPERTY()
	float ElapsedTime = 0.f;
};

USTRUCT(DisplayName="Party: Psychokinesis")
struct FPCTask_PK : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FPCTask_PKInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};