// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossSuperArmorComponent.h"

UBossSuperArmorComponent::UBossSuperArmorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
 
	// 기본 Phase별 배율 설정
	PhaseArmorMultiplier.Add(EBossPhase::Phase1, 1.0f);
	PhaseArmorMultiplier.Add(EBossPhase::Phase2, 1.5f);
	PhaseArmorMultiplier.Add(EBossPhase::Phase2_Enhanced, 2.0f);
	PhaseArmorMultiplier.Add(EBossPhase::Phase3_Cutscene, 2.5f);
}
 
void UBossSuperArmorComponent::ActivateSuperArmor(float Duration)
{
	if (bSuperArmorActive)
	{
		return;
	}
 
	bSuperArmorActive = true;
	AccumulatedDamage = 0.f;
	SuperArmorTimer = Duration;
 
	// Duration이 양수면 자동 해제를 위해 Tick 활성화
	if (Duration > 0.f)
	{
		SetComponentTickEnabled(true);
	}
 
	OnSuperArmorChanged.Broadcast(true);
 
	UE_LOG(LogTemp, Log, TEXT("[SuperArmor] 활성화 (Duration: %.1f)"),
		Duration);
}
 
void UBossSuperArmorComponent::DeactivateSuperArmor()
{
	if (!bSuperArmorActive)
	{
		return;
	}
 
	bSuperArmorActive = false;
	AccumulatedDamage = 0.f;
	SuperArmorTimer = -1.f;
	SetComponentTickEnabled(false);
 
	OnSuperArmorChanged.Broadcast(false);
 
	UE_LOG(LogTemp, Log, TEXT("[SuperArmor] 비활성화"));
}
 
bool UBossSuperArmorComponent::ShouldAbsorbHit(
	float IncomingDamage,
	EHitReactionType ReactionType) const
{
	if (!bSuperArmorActive)
	{
		return false;
	}
 
	// Flinch는 항상 무시 옵션
	if (bAlwaysIgnoreFlinch && ReactionType == EHitReactionType::Flinch)
	{
		return true;
	}
 
	// 누적 대미지가 임계값을 넘으면 슈퍼아머 파괴
	if (AccumulatedDamage + IncomingDamage >= SuperArmorBreakThreshold)
	{
		// const_cast는 이상적이지 않지만, 슈퍼아머 파괴는 즉각 반영 필요
		// 실제 구현에서는 Mutable 패턴이나 별도 이벤트로 처리 권장
		UE_LOG(LogTemp, Log,
			TEXT("[SuperArmor] 파괴! (누적: %.0f + 신규: %.0f >= 임계: %.0f)"),
			AccumulatedDamage, IncomingDamage, SuperArmorBreakThreshold);
		return false;
	}
 
	// 슈퍼아머가 대미지를 흡수
	// 대미지 자체는 여전히 적용되지만, 피격 리액션만 무시됨
	const_cast<UBossSuperArmorComponent*>(this)->AccumulatedDamage += IncomingDamage;
	return true;
}
 
void UBossSuperArmorComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
 
	if (!bSuperArmorActive || SuperArmorTimer < 0.f)
	{
		return;
	}
 
	SuperArmorTimer -= DeltaTime;
	if (SuperArmorTimer <= 0.f)
	{
		DeactivateSuperArmor();
	}
}

