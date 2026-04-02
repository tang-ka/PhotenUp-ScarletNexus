// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget/PlayerHUDViewModel.h"

void UPlayerHUDViewModel::SetHP(int32 Current)
{
	CurrentHP = Current;
	OnHPUpdated.Broadcast(CurrentHP, MaxHP > 0 ? static_cast<float>(CurrentHP) / MaxHP : 0.f);
}

void UPlayerHUDViewModel::SetMaxHP(int32 Max)
{
	MaxHP = Max;
	OnMaxHPUpdated.Broadcast(MaxHP);
	OnHPUpdated.Broadcast(CurrentHP, MaxHP > 0 ? static_cast<float>(CurrentHP) / MaxHP : 0.f);
}

void UPlayerHUDViewModel::SetMP(int32 Current)
{
	CurrentMP = Current;
	OnMPUpdated.Broadcast(CurrentMP, MaxMP > 0 ? static_cast<float>(CurrentMP) / MaxMP : 0.f);
}

void UPlayerHUDViewModel::SetMaxMP(int32 Max)
{
	MaxMP = Max;
	OnMPUpdated.Broadcast(CurrentMP, MaxMP > 0 ? static_cast<float>(CurrentMP) / MaxMP : 0.f);
}