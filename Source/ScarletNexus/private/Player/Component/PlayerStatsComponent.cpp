// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PlayerStatsComponent.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	CurrentMP = MaxMP;
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
	SetCurrentHP(CurrentHP - DamageAmount);
}

