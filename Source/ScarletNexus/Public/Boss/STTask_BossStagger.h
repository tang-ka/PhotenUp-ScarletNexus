// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_BossStagger.generated.h"
 
// 그로기 상태의 서브 페이즈
UENUM(BlueprintType)
enum class EStaggerSubPhase : uint8
{
	// 경직 진입 모션 재생 중
	EnterStagger,
	// 그로기 상태 유지 (플레이어가 브레인 크래시 입력 대기)
	StaggerLoop,
	// 브레인 크래시 발동 (QTE 또는 자동)
	BrainCrush,
	// 그로기 해제 (시간 초과 시)
	RecoverFromStagger,
};
 
USTRUCT()
struct FSTTask_BossStaggerInstanceData
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere, Category = "Input")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
 
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bBrainCrushAvailable = false;
 
	// 그로기 경과 시간
	UPROPERTY()
	float ElapsedTime = 0.f;
 
	// 현재 서브 페이즈
	UPROPERTY()
	EStaggerSubPhase SubPhase = EStaggerSubPhase::EnterStagger;
 
	// 브레인 크래시가 발동되었는지
	UPROPERTY()
	bool bBrainCrushTriggered = false;
 
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> ContextActor = nullptr;
};
 
USTRUCT(DisplayName = "Boss stagger / brain crush")
struct SCARLETNEXUS_API FSTTask_BossStagger : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_BossStaggerInstanceData;
 
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
 
	// 그로기 진입 몽타주
	UPROPERTY(EditAnywhere, Category = "Config|Montages")
	TObjectPtr<UAnimMontage> StaggerEnterMontage = nullptr;
 
	// 그로기 루프 몽타주 (반복 재생)
	UPROPERTY(EditAnywhere, Category = "Config|Montages")
	TObjectPtr<UAnimMontage> StaggerLoopMontage = nullptr;
 
	// 그로기 해제 몽타주
	UPROPERTY(EditAnywhere, Category = "Config|Montages")
	TObjectPtr<UAnimMontage> StaggerRecoverMontage = nullptr;
 
	// 브레인 크래시 피격 몽타주 (보스 시점)
	UPROPERTY(EditAnywhere, Category = "Config|Montages")
	TObjectPtr<UAnimMontage> BrainCrushReceiveMontage = nullptr;
 
	// 그로기 유지 최대 시간 (초과 시 자동 해제)
	UPROPERTY(EditAnywhere, Category = "Config")
	float MaxStaggerDuration = 5.f;
 
	// 브레인 크래시 입력 가능 시간 (그로기 진입 후 이 시간 내에 입력해야 함)
	UPROPERTY(EditAnywhere, Category = "Config")
	float BrainCrushWindowDuration = 4.f;
 
	// 브레인 크래시 보너스 대미지 비율 (MaxHP 대비)
	UPROPERTY(EditAnywhere, Category = "Config")
	float BrainCrushDamageRatio = 0.15f;
 
private:
	void TransitionToSubPhase(
		FInstanceDataType& InstanceData,
		ACharacter* BossChar,
		EStaggerSubPhase NewSubPhase) const;
};