// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossOverheadWidget.h"
#include "Interface/Damageable.h"
 
void UBossOverheadWidget::SetOwnerBoss(AActor* InBossActor)
{
	OwnerBoss = InBossActor;
}
 
float UBossOverheadWidget::GetHPPercent() const
{
	if (!OwnerBoss || !OwnerBoss->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		return 0.f;
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

