// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget/BossHUDViewModel.h"

void UBossHUDViewModel::SetInitialState(float InCurrentHP, float InMaxHP)
{
	CachedMaxHP     = InMaxHP;
	CachedCurrentHP = InCurrentHP;

	const float Percent = CachedMaxHP > 0.f ? CachedCurrentHP / CachedMaxHP : 0.f;
	OnHPUpdated.Broadcast(CachedCurrentHP, CachedMaxHP, Percent);
}

void UBossHUDViewModel::HandleBossHPChanged(float InCurrentHP, float InMaxHP, float DamageAmount)
{
	CachedCurrentHP = InCurrentHP;
	CachedMaxHP     = InMaxHP;

	const float Percent = CachedMaxHP > 0.f ? CachedCurrentHP / CachedMaxHP : 0.f;
	OnHPUpdated.Broadcast(CachedCurrentHP, CachedMaxHP, Percent);
}

void UBossHUDViewModel::HandleBossPhaseChanged(EBossPhase OldPhase, EBossPhase NewPhase)
{
	CachedPhase = NewPhase;
	OnPhaseUpdated.Broadcast(OldPhase, NewPhase);
}

