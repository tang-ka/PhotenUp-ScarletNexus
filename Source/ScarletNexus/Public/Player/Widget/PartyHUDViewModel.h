// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Widget/ViewModel.h"
#include "PartyHUDViewModel.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPartyHPUpdatedDelegate, int32 /*Current*/, float /*Percent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyMaxHPUpdatedDelegate, int32 /*Max*/);

/**
 * 파티 캐릭터 스탯 위젯을 구동하는 ViewModel
 */
UCLASS()
class SCARLETNEXUS_API UPartyHUDViewModel : public UViewModel
{
	GENERATED_BODY()

public:
	void SetHP(int32 Current);
	void SetMaxHP(int32 Max);

public:
	FOnPartyHPUpdatedDelegate OnHPUpdated;
	FOnPartyMaxHPUpdatedDelegate OnMaxHPUpdated;

private:
	UPROPERTY() int32 CurrentHP{0};
	UPROPERTY() int32 MaxHP{0};
};

