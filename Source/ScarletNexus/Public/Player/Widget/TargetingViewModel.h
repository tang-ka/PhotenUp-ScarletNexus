// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViewModel.h"
#include "TargetingViewModel.generated.h"

class UPlayerPerceptionComponent;

// Widget이 구독하는 HardTarget 변경 이벤트 (nullptr = 숨김)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHardTargetUpdated, AActor*);
// Widget이 구독하는 PsychokinesisTarget 변경 이벤트 (nullptr = 숨김)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPKTargetUpdated, AActor*);

/**
 * PlayerPerceptionComponent(Model) 와 TargetingUIWidget(View) 사이의 중간 허브.
 * Model 델리게이트를 구독하고, Widget이 소비하는 델리게이트를 브로드캐스트한다.
 * Widget은 이 클래스만 알고, PerceptionComponent 및 PlayerCharacterBase를 일절 모른다.
 */
UCLASS()
class SCARLETNEXUS_API UTargetingViewModel : public UViewModel
{
	GENERATED_BODY()

public:
	/** PerceptionComponent 델리게이트 구독 시작. 이미 구독 중이면 먼저 해제 후 재구독 */
	void Initialize(UPlayerPerceptionComponent* InPerceptionComp);

	/** 델리게이트 구독 해제 및 내부 상태 초기화 */
	void Shutdown();

	/** 현재 HardTarget 반환 (락온 비활성화 시 nullptr) */
	AActor* GetHardTarget() const
	{
		return HardTarget.IsValid() ? HardTarget.Get() : nullptr;
	}

	/** 현재 PsychokinesisTarget 반환 */
	AActor* GetPKTarget() const
	{
		return PKTarget.IsValid() ? PKTarget.Get() : nullptr;
	}

public:
	// Widget이 바인딩하는 아웃바운드 델리게이트
	FOnHardTargetUpdated OnHardTargetUpdated;
	FOnPKTargetUpdated OnPKTargetUpdated;

private:
	/** PerceptionComponent::OnHardTargetChanged 수신 핸들러 */
	void HandleHardTargetChanged(AActor* NewTarget);

	/** PerceptionComponent::OnPsychokinesisTargetChanged 수신 핸들러 */
	void HandlePKTargetChanged(AActor* NewTarget);

private:
	TWeakObjectPtr<UPlayerPerceptionComponent> PerceptionComp;
	TWeakObjectPtr<AActor> HardTarget;
	TWeakObjectPtr<AActor> PKTarget;

	// Shutdown 시 구독 해제를 위한 핸들 보관
	FDelegateHandle HardTargetHandle;
	FDelegateHandle PKTargetHandle;
};

