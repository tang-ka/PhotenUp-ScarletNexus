// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Component/PlayerCharacterMovementComponent.h"
#include "Player/PlayerCharacterBase.h"

void UPlayerCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (APlayerCharacterBase* Character = Cast<APlayerCharacterBase>(GetCharacterOwner()))
	{
		Character->OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	}
}

