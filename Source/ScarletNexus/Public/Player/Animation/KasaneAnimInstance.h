// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KasaneAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	None UMETA(DisplayName = "None"),
	A1 UMETA(DisplayName = "A1"),
	A2 UMETA(DisplayName = "A2"),
	A3 UMETA(DisplayName = "A3"),
};

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
	void SetIsBasicAttacking(bool bAttacking) { bIsBasicAttacking = bAttacking; }
	void SetAttackState(EAttackState InState);

	UFUNCTION()
	void AnimNotify_JumpStart();
	
	UFUNCTION()
	void AnimNotify_JumpEnd();
	
	UFUNCTION()
	void AnimNotify_CriticalHit();
	
	UFUNCTION()
	void AnimNotify_AllowMove();

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	bool bIsBasicAttacking = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	EAttackState AttackState = EAttackState::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess=true))
	float BlendWeight = 1;
};
