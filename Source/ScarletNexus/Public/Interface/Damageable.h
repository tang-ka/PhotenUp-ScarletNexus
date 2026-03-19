// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Damage")
	TObjectPtr<AActor> DamageCauser{};

	UPROPERTY(BlueprintReadWrite, Category="Damage")
	int DamageAmount{};
};

// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCARLETNEXUS_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	bool ReceiveDamage (FDamageInfo DamageInfo);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	int GetHP() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	float GetHPPercent() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	bool IsDead() const;
};
