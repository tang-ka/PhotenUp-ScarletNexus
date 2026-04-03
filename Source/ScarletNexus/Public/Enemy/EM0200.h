// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EM0200.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API AEM0200 : public AEnemyBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();
};
