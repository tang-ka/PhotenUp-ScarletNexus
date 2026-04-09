// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Widget/PartyHUDViewModel.h"

void UPartyHUDViewModel::SetHP(int32 Current)
{
	CurrentHP = Current;
	const float Percent = (MaxHP > 0) ? static_cast<float>(CurrentHP) / static_cast<float>(MaxHP) : 0.f;
	OnHPUpdated.Broadcast(CurrentHP, Percent);
}

void UPartyHUDViewModel::SetMaxHP(int32 Max)
{
	MaxHP = Max;
	OnMaxHPUpdated.Broadcast(MaxHP);
}

