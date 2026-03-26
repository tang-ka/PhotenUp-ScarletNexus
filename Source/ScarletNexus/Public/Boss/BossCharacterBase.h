// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "IStaggerable.h"
#include "ICombatState.h"
#include "BossTypes.h"
#include "BossCharacterBase.generated.h"
 
class UBossConfigDataAsset;
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossPhaseChanged,
	EBossPhase, OldPhase,
	EBossPhase, NewPhase);
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnBossHPChanged,
	float, CurrentHP,
	float, MaxHP,
	float, DamageAmount);
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossStaggerChanged,
	float, CurrentStagger,
	float, MaxStagger);
 
UCLASS()
class SCARLETNEXUS_API ABossCharacterBase : public ACharacter,
	public IDamageable, public IStaggerable, public ICombatState
{
	GENERATED_BODY()
 
public:
	ABossCharacterBase();
 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
 
	UFUNCTION(BlueprintCallable, Category = "Boss")
	EBossPhase GetCurrentPhase() const { return CurrentPhase; }
 
	// ============================================================
	// IDamageable 구현 (데미지 + HP 조회)
	// ============================================================
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	virtual int GetHP_Implementation() const override;
	virtual float GetHPPercent_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
 
	// ============================================================
	// IStaggerable 구현
	// ============================================================
	virtual float GetStaggerRatio_Implementation() const override;
	virtual void ApplyStaggerDamage_Implementation(float StaggerAmount) override;
	virtual bool IsStaggered_Implementation() const override;
 
	// ============================================================
	// ICombatState 구현
	// ============================================================
	virtual bool IsAttacking_Implementation() const override;
	virtual bool HasSuperArmor_Implementation() const override;
 
	// ============================================================
	// 보스 전용 (InitializeWithConfig)
	// ============================================================
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void InitializeWithConfig(UBossConfigDataAsset* Config);
 
	// ============================================================
	// Delegates
	// ============================================================
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossPhaseChanged OnPhaseChanged;
 
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossHPChanged OnHPChanged;
 
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossStaggerChanged OnStaggerChanged;
 
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Config")
	TObjectPtr<UBossConfigDataAsset> BossConfig;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxHPValue = 10000.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHPValue = 10000.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxStaggerGauge = 100.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentStaggerGauge = 0.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float StaggerDecayRate = 5.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float StaggerDecayDelay = 3.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossCombatState CurrentCombatState = EBossCombatState::Idle;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	bool bSuperArmor = false;
 
	float LastStaggerHitTime = 0.f;
 
	void CheckPhaseTransition();
	void UpdateStaggerDecay(float DeltaTime);
 
	UFUNCTION()
	void HandleDeath();
};