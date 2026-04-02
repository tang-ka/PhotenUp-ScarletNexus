// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "EnemyBase.generated.h"

UCLASS()
class SCARLETNEXUS_API AEnemyBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

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
	// 스턴
	void Stun(float Duration);
	// Die
	UFUNCTION()
	virtual void OnDie();

#pragma region IDamageable
	bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	int GetHP_Implementation() const override;
	float GetHPPercent_Implementation() const override;
	bool IsDead_Implementation() const override;
#pragma endregion 
};
