// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectMacros.h"
#include "PartyCharacterStateContext.generated.h"

class APlayerCharacterBase;
class APKObject;

USTRUCT(BlueprintType)
struct FPartyCharacterStateContext
{
	GENERATED_BODY()

	// Chase 대상 파티원
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> TrackedParty = nullptr;

	UPROPERTY()
	TObjectPtr<APKObject> FoundPKObject = nullptr;

	UPROPERTY(EditAnywhere)
	float AttackRange = 500.f;

	UPROPERTY()
	bool bCoolDown = false;
};
