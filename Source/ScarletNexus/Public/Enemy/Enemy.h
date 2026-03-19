// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class SCARLETNEXUS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 스테이터스
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int MaxHP = 250;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrHP = MaxHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float DetectRange = 300.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AttackRange = 100.f;
	
	// 타겟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor;
	
	// **행동 트리
	// 공격
	UFUNCTION()
	virtual void OnAttack();
	// 피격
	UFUNCTION()
	virtual void OnHit();
	// HP 처리
	UFUNCTION()
	void TakeDamage(int DamageValue);
	// 스턴
	void Stun(float Duration);
	// Die
	UFUNCTION()
	virtual void OnDie();
};
