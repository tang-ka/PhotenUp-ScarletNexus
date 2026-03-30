// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	bool CanBePickeduped() const;
	// 집어 올려지기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	void OnPKPickuped();
	// 다시 놓아지기 (집기 해제)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	void OnPKReleased();
	// 집혀졌을 때 던져지기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	void OnPKThrown(const FVector& ThrowDir, float ThrowForce);
	// 잡혀졌을 때 던져지기 (Physics Simulated On)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category=PK)
	void OnPKThrownPS(const FVector& ThrowDir, float ThrowForce);
};
