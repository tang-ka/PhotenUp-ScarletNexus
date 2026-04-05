// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PlayerStatsComponent.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentHP(MaxHP);
	SetCurrentMP(MaxMP);
}

void UPlayerStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UPlayerStatsComponent::GetHPPercentage() const
{
	return static_cast<float>(CurrentHP) / MaxHP;
}

void UPlayerStatsComponent::HealAmount(int32 HealAmount)
{
	SetCurrentHP(CurrentHP + HealAmount);
}

void UPlayerStatsComponent::HealPercent(float HealPercent)
{
	SetCurrentHP(CurrentHP * (1 + HealPercent));
}

void UPlayerStatsComponent::ReceiveDamage(int32 DamageAmount)
{
	if (IsDead())
	{
		return;
	}

	SetCurrentHP(CurrentHP - DamageAmount);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

void UPlayerStatsComponent::SetCurrentHP(int32 NewHP)
{ 
	CurrentHP = FMath::Clamp(NewHP, 0, MaxHP);
	OnHPChanged.Broadcast(CurrentHP);
}

void UPlayerStatsComponent::SetMaxHP(int32 NewMaxHP)
{
	MaxHP = FMath::Max(1, NewMaxHP);
	OnMaxHPChanged.Broadcast(MaxHP);
	SetCurrentHP(CurrentHP);
}

void UPlayerStatsComponent::SetCurrentMP(int32 NewMP)
{ 
	CurrentMP = FMath::Clamp(NewMP, 0, MaxMP); 
	OnMPChanged.Broadcast(CurrentMP);
}

void UPlayerStatsComponent::SetMaxMP(int32 NewMaxMP)
{
	MaxMP = FMath::Max(1, NewMaxMP);
	SetCurrentMP(CurrentMP);
}
