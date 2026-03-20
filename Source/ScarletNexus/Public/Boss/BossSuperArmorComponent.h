// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossTypes.h"
#include "BossSuperArmorComponent.generated.h"
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSuperArmorChanged, bool, bIsActive);
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuperArmorBroken);
 
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCARLETNEXUS_API UBossSuperArmorComponent : public UActorComponent
{
	GENERATED_BODY()
 
public:
	UBossSuperArmorComponent();
 
	
	// 슈퍼아머 제어
 
	// 슈퍼아머 활성화 (특정 공격 모션 시작 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Boss|SuperArmor")
	void ActivateSuperArmor(float Duration = -1.f);
 
	// 슈퍼아머 비활성화
	UFUNCTION(BlueprintCallable, Category = "Boss|SuperArmor")
	void DeactivateSuperArmor();
 
	// 현재 슈퍼아머 활성 여부
	UFUNCTION(BlueprintPure, Category = "Boss|SuperArmor")
	bool IsSuperArmorActive() const { return bSuperArmorActive; }
 
	
	// 피격 판정에서 호출
 
	// 피격 시 슈퍼아머가 데미지를 흡수할지 결정
	// 반환값: true면 피격 리액션 무시, false면 정상 피격
	UFUNCTION(BlueprintCallable, Category = "Boss|SuperArmor")
	bool ShouldAbsorbHit(float IncomingDamage, EHitReactionType ReactionType) const;
 
	
	UPROPERTY(BlueprintAssignable, Category = "Boss|SuperArmor")
	FOnSuperArmorChanged OnSuperArmorChanged;
 
	// 슈퍼아머가 깨졌을 때 (누적 대미지 초과)
	UPROPERTY(BlueprintAssignable, Category = "Boss|SuperArmor")
	FOnSuperArmorBroken OnSuperArmorBroken;
 
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
 
	
	// 설정
 
	// 슈퍼아머 중 받을 수 있는 최대 누적 대미지 (초과 시 슈퍼아머 파괴)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float SuperArmorBreakThreshold = 500.f;
 
	// Phase별 슈퍼아머 강도 배율
	// Phase 3에서는 슈퍼아머가 더 잘 깨지지 않음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TMap<EBossPhase, float> PhaseArmorMultiplier;
 
	// Flinch(약한 피격)는 항상 슈퍼아머로 무시할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bAlwaysIgnoreFlinch = true;
 
	
	// 런타임 상태
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	bool bSuperArmorActive = false;
 
	// 슈퍼아머 활성 중 누적된 대미지
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	float AccumulatedDamage = 0.f;
 
	// 슈퍼아머 자동 해제 타이머 (-1이면 수동 해제)
	float SuperArmorTimer = -1.f;
};