// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossHUDWidget.h"
#include "Player/Widget/BossHUDViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBossHUDWidget::InitViewModel(UViewModel* InViewModel)
{
	Super::InitViewModel(InViewModel);

	UBossHUDViewModel* VM = Cast<UBossHUDViewModel>(OwnerViewModel);
	if (!VM) return;

	// ViewModel 델리게이트 구독
	VM->OnHPUpdated.AddUObject(this, &UBossHUDWidget::UpdateHP);
	VM->OnPhaseUpdated.AddUObject(this, &UBossHUDWidget::UpdatePhase);

	// 위젯 생성 시점에 ViewModel 캐시값으로 초기 동기화
	UpdateHP(VM->GetCachedCurrentHP(), VM->GetCachedMaxHP(), VM->GetCachedHPPercent());
	UpdatePhase(VM->GetCachedPhase(), VM->GetCachedPhase());
}

void UBossHUDWidget::UpdateHP(float CurrentHP, float MaxHP, float Percent)
{
	CachedCurrentHP = CurrentHP;
	CachedMaxHP     = MaxHP;
	CachedHPPercent = Percent;
	bIsBossAlive    = (CurrentHP > 0.f);

	if (pb_BossHP)
	{
		pb_BossHP->SetPercent(Percent);
	}
}

void UBossHUDWidget::UpdatePhase(EBossPhase /*OldPhase*/, EBossPhase NewPhase)
{
	CachedPhase = NewPhase;

	if (txt_Phase)
	{
		txt_Phase->SetText(GetPhaseText());
	}
}

FText UBossHUDWidget::GetHPText() const
{
	return FText::FromString(
		FString::Printf(TEXT("%.0f / %.0f"), CachedCurrentHP, CachedMaxHP));
}

FText UBossHUDWidget::GetPhaseText() const
{
	switch (CachedPhase)
	{
	case EBossPhase::Phase1:          return FText::FromString(TEXT("Phase 1"));
	case EBossPhase::Phase2:          return FText::FromString(TEXT("Phase 2"));
	case EBossPhase::Phase2_Enhanced: return FText::FromString(TEXT("Phase 2-2"));
	case EBossPhase::Phase3_Cutscene: return FText::FromString(TEXT("Phase 3"));
	default:                          return FText::FromString(TEXT("---"));
	}
}
