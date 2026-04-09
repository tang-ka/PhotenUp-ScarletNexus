// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_BladeGlow.generated.h"

/**
 * 애니메이션 구간 동안 블레이드 머티리얼을 Glow 상태로 전환합니다.
 * 구간 시작: UBladeHandlerComponent::OnBladeGlowBegin()
 * 구간 종료: UBladeHandlerComponent::OnBladeGlowEnd()
 */
UCLASS(DisplayName = "Blade Glow")
class SCARLETNEXUS_API UAnimNotifyState_BladeGlow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override { return TEXT("BladeGlow"); }

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration,
	                         const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};

