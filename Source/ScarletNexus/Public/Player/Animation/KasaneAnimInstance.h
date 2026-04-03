// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KasaneAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UKasaneAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	bool IsJumEnd() const { return bIsJumpEnd; }
	
	void SetSpeed(float InSpeed) { Speed = InSpeed; }
	void SetIsInAir(bool bInAir) { bIsInAir = bInAir; }
	void SetIsFalling(bool bFalling) { bIsFalling = bFalling; }
	void SetIsJumpEnd(bool bInJumpEnd) { bIsJumpEnd = bInJumpEnd; }
	
	UFUNCTION()
	void AnimNotify_JumpStart();
	
	UFUNCTION()
	void AnimNotify_JumpEnd();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	float Speed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	bool bIsInAir = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	bool bIsFalling = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	bool bIsJumpEnd = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	bool bIsDashing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	bool bIsStaggered = false;
};
