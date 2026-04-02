// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BuddyRummy.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API ABuddyRummy : public AEnemyBase
{
	GENERATED_BODY()
	
	virtual void OnAttack() override;
	virtual void OnHit() override;
	virtual void OnDie() override;
};
