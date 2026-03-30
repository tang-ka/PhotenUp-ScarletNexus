// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Component/ComboComponent.h"

#include "ScarletNexus.h"
#include "Data/ComboAttackDataAsset.h"


UComboComponent::UComboComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UComboComponent::TryExecuteCombo(EAttackType InputType)
{
	TSoftObjectPtr<UComboAttackDataAsset>* FoundAttackDA = nullptr;
	// 현재 콤보 중이라면, CurrentAttack의 NextComboMap에서 입력에 해당하는 다음 공격을 찾는다.
	if (IsInCombo())
	{
		FoundAttackDA = CurrentAttack->NextComboMap.Find(InputType);
		// 다음 공격이 없다면 콤보 실패로 간주하고 초기화한다.
		if (!FoundAttackDA)
		{
			ResetCombo();
		}
	}
	
	// 콤보 중이 아니라면, RootAttackMap에서 입력에 해당하는 첫 공격을 찾는다.
	if (!IsInCombo())
	{
		FoundAttackDA = RootAttackMap.Find(InputType);
	}
	
	UComboAttackDataAsset* NextAttackDA = nullptr;
	if (FoundAttackDA && !FoundAttackDA->IsNull())
	{
		NextAttackDA = LoadAttackDataAsset(*FoundAttackDA);
	}

	if (!NextAttackDA)
	{
		PRINTLOG_SH(TEXT("[Combo] 매칭 실패: 콤보 %d단계"), CurComboIndex);
		return false;
	}

	CurrentAttack = NextAttackDA;
	++CurComboIndex;

	PRINTLOG_SH(TEXT("[Combo] 콤보 진행: %s | %d단계 | 몽타주: %s"),
	            *CurrentAttack->AttackID.ToString(),
	            CurComboIndex,
	            CurrentAttack->AttackMontage ? *CurrentAttack->AttackMontage->GetName() : TEXT("None"));

	OnComboExecuted.Broadcast(CurrentAttack);
	return true;
}

void UComboComponent::ResetCombo()
{
	if (CurComboIndex > 0)
	{
		PRINTLOG_SH(TEXT("[Combo] 콤보 리셋: %d단계에서 초기화"), CurComboIndex);
	}

	CurrentAttack = nullptr;
	CurComboIndex = 0;
	OnComboReset.Broadcast();
}

UComboAttackDataAsset* UComboComponent::LoadAttackDataAsset(const TSoftObjectPtr<UComboAttackDataAsset>& SoftPtr) const
{
	if (SoftPtr.IsNull())
	{
		PRINTLOG_SH(TEXT("[Combo] ResolveAttack: SoftPtr is null"));
		return nullptr;
	}

	UComboAttackDataAsset* LoadedAsset = SoftPtr.LoadSynchronous();

	if (!LoadedAsset)
	{
		PRINTLOG_SH(TEXT("[Combo] ResolveAttack: Failed to load asset from SoftPtr"));
	}

	return LoadedAsset;
}
