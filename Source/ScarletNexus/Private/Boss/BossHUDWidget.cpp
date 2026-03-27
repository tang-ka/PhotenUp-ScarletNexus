// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossHUDWidget.h"
#include "Boss/BossCharacterBase.h"
#include "Interface/Damageable.h"
 
void UBossHUDWidget::SetBossActor(AActor* InBossActor)
{
	BossActor = InBossActor;
}
 
float UBossHUDWidget::GetHPPercent() const
{
	if (!BossActor || !BossActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		return 0.f;
	}
	return IDamageable::Execute_GetHPPercent(BossActor);
}
 
FText UBossHUDWidget::GetHPText() const
{
	if (!BossActor || !BossActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		return FText::FromString(TEXT("0 / 0"));
	}
 
	const int32 CurrentHP = IDamageable::Execute_GetHP(BossActor);
 
	// MaxHP는 BossCharacterBase에서 가져옴
	int32 MaxHP = 10000;
	if (const ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(BossActor))
	{
		MaxHP = static_cast<int32>(BossChar->GetHP_Implementation() / FMath::Max(BossChar->GetHPPercent_Implementation(), 0.01f));
	}
 
	return FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentHP, MaxHP));
}
 
FText UBossHUDWidget::GetPhaseText() const
{
	if (const ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(BossActor))
	{
		switch (BossChar->GetCurrentPhase())
		{
		case EBossPhase::Phase1:          return FText::FromString(TEXT("Phase 1"));
		case EBossPhase::Phase2:          return FText::FromString(TEXT("Phase 2"));
		case EBossPhase::Phase2_Enhanced: return FText::FromString(TEXT("Phase 2-2"));
		case EBossPhase::Phase3_Cutscene: return FText::FromString(TEXT("Phase 3"));
		}
	}
	return FText::FromString(TEXT("---"));
}
 
FText UBossHUDWidget::GetBossName() const
{
	return BossDisplayName;
}
 
bool UBossHUDWidget::IsBossValid() const
{
	return IsValid(BossActor);
}
 
bool UBossHUDWidget::IsBossDead() const
{
	if (!BossActor || !BossActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		return true;
	}
	return IDamageable::Execute_IsDead(BossActor);
}
