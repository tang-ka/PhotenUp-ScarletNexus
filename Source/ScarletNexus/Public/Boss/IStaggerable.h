// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IStaggerable.generated.h"
 
UINTERFACE(BlueprintType)
class UStaggerable : public UInterface
{
	GENERATED_BODY()
};
 
class SCARLETNEXUS_API IStaggerable
{
	GENERATED_BODY()
 
public:
	// 경직 게이지 비율 (0.0 ~ 1.0)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stagger")
	float GetStaggerRatio() const;
 
	// 경직 데미지 적용
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stagger")
	void ApplyStaggerDamage(float StaggerAmount);
 
	// 현재 경직 상태인지
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stagger")
	bool IsStaggered() const;
};
