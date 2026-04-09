// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossOverheadWidget.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Interface/Damageable.h"
 
void UBossOverheadWidget::SetOwnerBoss(AActor* InBossActor)
{
	OwnerBoss = InBossActor;
	CachedHPPercent = 1.f;
	
	if (ProgressBarHP)
	{
		ProgressBarHP->SetPercent(CachedHPPercent);
	}
}
 
float UBossOverheadWidget::GetHPPercent() const
{
	if (!OwnerBoss || !OwnerBoss->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		return CachedHPPercent;
	}
	return IDamageable::Execute_GetHPPercent(OwnerBoss);
}

void UBossOverheadWidget::SetTextName(const FText& Name)
{
	if (TextName)
	{
		TextName->SetText(Name);
	}
}

void UBossOverheadWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 위젯 생성시 HP 만땅 초기화
	CachedHPPercent = 1.f;
	// pb
	if (ProgressBarHP)
	{
		ProgressBarHP->SetPercent(CachedHPPercent);
	}
}

void UBossOverheadWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// HP 비율 프로그래스바에 반영
	const float newPercent = GetHPPercent();
	if (!FMath::IsNearlyEqual(CachedHPPercent, newPercent, 0.001f))
	{
		CachedHPPercent = newPercent;
		if (ProgressBarHP)
		{
			ProgressBarHP->SetPercent(CachedHPPercent);
		}
	}
}
