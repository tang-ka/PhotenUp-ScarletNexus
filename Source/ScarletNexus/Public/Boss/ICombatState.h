// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICombatState.generated.h"

UINTERFACE(BlueprintType)
class UCombatState : public UInterface
{
	GENERATED_BODY()
};
 
class SCARLETNEXUS_API ICombatState
{
	GENERATED_BODY()
 
public:
	// 공격 중인지
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool IsAttacking() const;
 
	// 슈퍼아머 활성 여부
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool HasSuperArmor() const;
};
