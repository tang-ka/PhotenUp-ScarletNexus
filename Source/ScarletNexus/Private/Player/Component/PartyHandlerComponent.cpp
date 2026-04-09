// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Component/PartyHandlerComponent.h"
#include "PartyAI/PartyMemberBase.h"

UPartyHandlerComponent::UPartyHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPartyHandlerComponent::AddPartyMember(APartyMemberBase* Member)
{
	if (!Member)
	{
		return false;
	}
	if (IsFull())
	{
		UE_LOG(LogTemp, Warning, TEXT("PartyHandlerComponent: 파티가 가득 찼습니다. (최대 %d명)"), MaxPartySize);
		return false;
	}
	if (IsInParty(Member))
	{
		UE_LOG(LogTemp, Warning, TEXT("PartyHandlerComponent: 이미 파티원입니다."));
		return false;
	}

	PartyPool.Add(Member);
	Member->ActivatePartyAI();
	OnPartyMemberAdded.Broadcast(Member);

	UE_LOG(LogTemp, Log, TEXT("PartyHandlerComponent: 파티원 추가 [%s], 현재 파티 인원: %d"),
		*Member->GetName(), PartyPool.Num());
	return true;
}

bool UPartyHandlerComponent::RemovePartyMember(APartyMemberBase* Member)
{
	if (!Member)
	{
		return false;
	}

	const int32 Index = PartyPool.IndexOfByKey(Member);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	PartyPool.RemoveAt(Index);
	Member->DeactivatePartyAI();
	OnPartyMemberRemoved.Broadcast(Member);

	UE_LOG(LogTemp, Log, TEXT("PartyHandlerComponent: 파티원 제거 [%s], 현재 파티 인원: %d"),
		*Member->GetName(), PartyPool.Num());
	return true;
}

APartyMemberBase* UPartyHandlerComponent::GetPartyMember(int32 Index) const
{
	if (!PartyPool.IsValidIndex(Index))
	{
		return nullptr;
	}
	return PartyPool[Index];
}

bool UPartyHandlerComponent::IsInParty(APartyMemberBase* Member) const
{
	return PartyPool.Contains(Member);
}

void UPartyHandlerComponent::ClearParty()
{
	// 복사본으로 순회하며 제거 (Remove 도중 배열 변경 방지)
	TArray<TObjectPtr<APartyMemberBase>> Temp = PartyPool;
	for (auto& M : Temp)
	{
		RemovePartyMember(M.Get());
	}
}

