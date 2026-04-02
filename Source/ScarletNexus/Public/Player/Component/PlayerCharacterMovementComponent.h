// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterMovementComponent.generated.h"

/**
 * UCharacterMovementComponent::OnMovementUpdated 를 오버라이드해서
 * APlayerCharacterBase::OnMovementUpdated 로 포워딩합니다.
 */
UCLASS()
class SCARLETNEXUS_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	
};

