// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossHitReaction.generated.h"



USTRUCT()
struct FSTTask_BossHitReactionInstanceData
{
	GENERATED_BODY()
 
 
	// 현재 페이즈 (페이즈별 슈퍼아머 판정에 사용)
	UPROPERTY(EditAnywhere, Category = "Input")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
 
 
	// 리액션 몽타주 종료 여부
	UPROPERTY()
	bool bReactionFinished = false;
 
	// 적용된 리액션 타입
	UPROPERTY()
	EHitReactionType AppliedReactionType = EHitReactionType::Flinch;
 
	// 피격 방향
	UPROPERTY()
	EHitDirection HitDirection = EHitDirection::Front;
 
	// Context
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
};
 
USTRUCT(DisplayName = "Boss hit reaction")
struct SCARLETNEXUS_API FSTTask_BossHitReaction : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossHitReactionInstanceData;
 
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
 
 
	// 피격 리액션 종류
	UPROPERTY(EditAnywhere, Category = "Config")
	EHitReactionType ReactionType = EHitReactionType::Flinch;
 
	// 방향별 피격 몽타주 (Front/Back/Left/Right 순서)
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<TObjectPtr<UAnimMontage>> DirectionalMontages;
 
	// 방향 무관 범용 몽타주 (DirectionalMontages가 비어있을 때 사용)
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UAnimMontage> FallbackMontage = nullptr;
 
	// 넉백 시 밀려나는 힘
	UPROPERTY(EditAnywhere, Category = "Config",
		meta = (EditCondition = "ReactionType == EHitReactionType::Knockback"))
	float KnockbackForce = 800.f;
 
	// 에어본 시 띄우는 높이
	UPROPERTY(EditAnywhere, Category = "Config",
		meta = (EditCondition = "ReactionType == EHitReactionType::Airborne"))
	float LaunchHeight = 400.f;
 
private:
	// 피격 방향 계산
	EHitDirection CalculateHitDirection(
		const AActor* BossActor,
		const AActor* DamageCauser) const;
 
	// 방향에 맞는 몽타주 선택
	UAnimMontage* GetMontageForDirection(EHitDirection Direction) const;
};