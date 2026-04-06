// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EnemyAnimInstance.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// ** 이동 (BlendSpace 바인딩)
	// 지면 속도
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	float Speed = 0.f;

	// 이동 방향 ( -180 ~ 180 )
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	float Direction = 0.f;

	// 이동 중 여부
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	bool bIsMoving = false;

	// ** 상태 플래그
	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsDie = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsStunned = false;

	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsInAir = false;

	// ** 몽타주 재생
	UFUNCTION(BlueprintCallable, Category="Animation")
	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable, Category="Animation")
	void PlayHitReactMontage();

	UFUNCTION(BlueprintCallable, Category="Animation")
	void PlayDieMontage();

	// ** 몽타주 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimationMontage")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimationMontage")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimationMontage")
	TObjectPtr<UAnimMontage> DieMontage;

//private:
	UPROPERTY()
	TObjectPtr<AEnemyBase> OwnerEnemy;
};
