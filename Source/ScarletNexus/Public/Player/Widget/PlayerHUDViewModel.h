// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViewModel.h"
#include "PlayerHUDViewModel.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatUpdatedDelegate, int32, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxStatUpdatedDelegate, int32);

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UPlayerHUDViewModel : public UViewModel
{
	GENERATED_BODY()

public:
	void SetHP(int32 Current);
	void SetMaxHP(int32 Max);
	void SetMP(int32 Current);
	void SetMaxMP(int32 Max);

public:
	FOnStatUpdatedDelegate OnHPUpdated;
	FOnStatUpdatedDelegate OnMPUpdated;
	FOnMaxStatUpdatedDelegate OnMaxHPUpdated;

private:
	UPROPERTY() int32 CurrentHP{0};
	UPROPERTY() int32 MaxHP{0};
	UPROPERTY() int32 CurrentMP{0};
	UPROPERTY() int32 MaxMP{0};
};
