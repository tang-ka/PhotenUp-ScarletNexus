// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Widget/ViewModel.h"
#include "Boss/BossTypes.h"
#include "BossHUDViewModel.generated.h"

/**
 * BossHUDWidget 전용 ViewModel
 *
 * 델리게이트 체인:
 *   ABossCharacterBase::OnHPChanged    (Dynamic) --> HandleBossHPChanged()
 *       --> OnHPUpdated  (Multicast)  --> UBossHUDWidget::UpdateHP()
 *
 *   ABossCharacterBase::OnPhaseChanged (Dynamic) --> HandleBossPhaseChanged()
 *       --> OnPhaseUpdated (Multicast) --> UBossHUDWidget::UpdatePhase()
 */

/** (CurrentHP, MaxHP, Percent 0~1) */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnBossHPUpdatedDelegate, float, float, float);
/** (OldPhase, NewPhase) */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBossPhaseUpdatedDelegate, EBossPhase, EBossPhase);

UCLASS()
class SCARLETNEXUS_API UBossHUDViewModel : public UViewModel
{
	GENERATED_BODY()

public:
	/** 보스 등록 직후 현재 상태를 한 번 밀어 넣을 때 사용 */
	void SetInitialState(float InCurrentHP, float InMaxHP);

	/** ABossCharacterBase::OnHPChanged 에 AddDynamic 으로 바인딩 */
	UFUNCTION()
	void HandleBossHPChanged(float InCurrentHP, float InMaxHP, float DamageAmount);

	/** ABossCharacterBase::OnPhaseChanged 에 AddDynamic 으로 바인딩 */
	UFUNCTION()
	void HandleBossPhaseChanged(EBossPhase OldPhase, EBossPhase NewPhase);

	// -- 캐시 읽기 (InitViewModel 시점 초기 동기화용) --
	float      GetCachedCurrentHP()  const { return CachedCurrentHP; }
	float      GetCachedMaxHP()      const { return CachedMaxHP; }
	float      GetCachedHPPercent()  const { return CachedMaxHP > 0.f ? CachedCurrentHP / CachedMaxHP : 0.f; }
	EBossPhase GetCachedPhase()      const { return CachedPhase; }

public:
	/** 위젯이 구독하는 HP 변경 알림 */
	FOnBossHPUpdatedDelegate   OnHPUpdated;
	/** 위젯이 구독하는 페이즈 변경 알림 */
	FOnBossPhaseUpdatedDelegate OnPhaseUpdated;

private:
	UPROPERTY() float      CachedCurrentHP = 0.f;
	UPROPERTY() float      CachedMaxHP     = 0.f;
	EBossPhase             CachedPhase     = EBossPhase::Phase1;
};

