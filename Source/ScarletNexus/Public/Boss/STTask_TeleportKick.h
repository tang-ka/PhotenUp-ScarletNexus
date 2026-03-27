// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BossTypes.h"
#include "STTask_TeleportKick.generated.h"
 
// 텔레포트 킥 진행 단계
UENUM()
enum class ETeleportKickPhase : uint8
{
	// 사라지는 중 (투명해짐)
	Vanishing,
	// 텔레포트 이동 (순간이동)
	Teleporting,
	// 등장
	Appearing,
	// 킥 공격
	Kicking,
	// 완료
	Done,
};
 
USTRUCT()
struct FSTTask_TeleportKickInstanceData
{
	GENERATED_BODY()
 
	// StateTree에서 바인딩할 보스 액터
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<AActor> ContextActor = nullptr;
 
	// 내부 상태
	UPROPERTY()
	ETeleportKickPhase AttackPhase = ETeleportKickPhase::Vanishing;
 
	UPROPERTY()
	float PhaseTimer = 0.f;
 
	UPROPERTY()
	FVector TeleportTarget = FVector::ZeroVector;
 
	UPROPERTY()
	FVector OriginalLocation = FVector::ZeroVector;
 
	// 데미지 적용 여부
	UPROPERTY()
	bool bDamageApplied = false;
};
 
USTRUCT(DisplayName = "Teleport Kick Attack")
struct SCARLETNEXUS_API FSTTask_TeleportKick : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
 
	using FInstanceDataType = FSTTask_TeleportKickInstanceData;
 
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
 
	// --- 설정 ---
 
	// 사라지는 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Config")
	float VanishDuration = 0.2f;
 
	// 텔레포트 후 등장까지 딜레이 (초)
	UPROPERTY(EditAnywhere, Category = "Config")
	float AppearDelay = 0.1f;
 
	// 킥 공격 판정 시간 (초) - 이 시간 안에 데미지 적용
	UPROPERTY(EditAnywhere, Category = "Config")
	float KickDuration = 0.5f;
 
	// 킥 데미지
	UPROPERTY(EditAnywhere, Category = "Config")
	float KickDamage = 150.f;
 
	// 킥 판정 범위 (반경)
	UPROPERTY(EditAnywhere, Category = "Config")
	float KickRadius = 200.f;
 
	// 플레이어 뒤쪽으로 이동할 거리
	UPROPERTY(EditAnywhere, Category = "Config")
	float TeleportOffset = 250.f;
 
	// 킥 넉백 힘
	UPROPERTY(EditAnywhere, Category = "Config")
	float KnockbackForce = 50.f;
 
private:
	FVector CalculateTeleportTarget(const AActor* BossActor, const AActor* PlayerActor) const;
	void ApplyKickDamage(AActor* BossActor, const FVector& KickLocation) const;
};