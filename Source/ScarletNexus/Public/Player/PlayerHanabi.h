// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerCharacterBase.h"
#include "PlayerHanabi.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API APlayerHanabi : public APlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	// 물리
	// 염력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PKForce = 200.f;
};
