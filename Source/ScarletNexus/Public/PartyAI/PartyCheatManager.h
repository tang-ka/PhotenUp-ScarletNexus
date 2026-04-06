// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PartyCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UPartyCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	// 콘솔에서 "WatchParty 0" 입력
	UFUNCTION(Exec)
	void WatchParty(int32 PartyIndex);

	// 기본 카메라로 복귀
	UFUNCTION(Exec)
	void WatchSelf();

private:
	UPROPERTY()
	TObjectPtr<class ACameraActor> SpectatorCam;
};
