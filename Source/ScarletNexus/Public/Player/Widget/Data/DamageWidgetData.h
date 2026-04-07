// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageWidgetData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDamageWidgetData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 DamageAmount = 0.f;

	UPROPERTY()
	FVector WorldLocation = FVector::ZeroVector;
};