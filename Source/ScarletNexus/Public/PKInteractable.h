// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKObject.h"
#include "UObject/Interface.h"
#include "PKInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPKInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCARLETNEXUS_API IPKInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 집을 수 있는 상태
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	virtual bool CanBePickeduped() const = 0;
	// 집어 올려지기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	virtual void OnPKPickuped() = 0;
	// 다시 놓아지기 (집기 해제)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	virtual void OnPKReleased() = 0;
	// 집혀졌을 때 던져지기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	virtual void OnPKThrown(const FVector& ThrowDir, float ThrowForce) = 0;
};
