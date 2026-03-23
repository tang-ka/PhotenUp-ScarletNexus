// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartyAI/PartyMemberBase.h"
#include "PartyHanabi.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API APartyHanabi : public APartyMemberBase
{
	GENERATED_BODY()
public:
	APartyHanabi();

	// PK 콤포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK")
	TObjectPtr<class UPKComponent> PKComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PK")
	float PKForce = 200.f;
};
