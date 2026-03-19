// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IBossCharacterInterface.h"
#include "BossTypes.h"
#include "BossCharacterBase.generated.h"
 
class UBossConfigDataAsset;
 
// 페이즈 전환 시 브로드캐스트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossPhaseChanged,
	EBossPhase, OldPhase,
	EBossPhase, NewPhase);
 
// HP 변경 시 브로드캐스트 (UI 연동용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnBossHPChanged,
	float, CurrentHP,
	float, MaxHP,
	float, DamageAmount);
 
// 경직 게이지 변경 시
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossStaggerChanged,
	float, CurrentStagger,
	float, MaxStagger);
 
UCLASS()
class SCARLETNEXUS_API ABossCharacterBase : public ACharacter, public IBossCharacterInterface
{
	GENERATED_BODY()
 
public:
	ABossCharacterBase();
 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
 
	
	// IBossCharacterInterface 구현
	
	virtual float GetHPRatio_Implementation() const override;
	virtual float GetCurrentHP_Implementation() const override;
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;
	virtual float GetStaggerRatio_Implementation() const override;
	virtual void ApplyStaggerDamage_Implementation(float StaggerAmount) override;
	virtual bool IsStaggered_Implementation() const override;
	virtual EBossCombatState GetCombatState_Implementation() const override;
	virtual bool IsAttacking_Implementation() const override;
	virtual bool HasSuperArmor_Implementation() const override;
	virtual void InitializeWithConfig_Implementation(UBossConfigDataAsset* Config) override;
 
	
	// Delegates (이벤트)
	
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossPhaseChanged OnPhaseChanged;
 
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossHPChanged OnHPChanged;
 
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossStaggerChanged OnStaggerChanged;
 
protected:
	
	// Config
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Config")
	TObjectPtr<UBossConfigDataAsset> BossConfig;
 
	
	// Stats (런타임)
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxHP = 10000.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHP = 10000.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxStaggerGauge = 100.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentStaggerGauge = 0.f;
 
	// 경직 게이지 자연 감소 속도 (초당)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float StaggerDecayRate = 5.f;
 
	// 경직 게이지 감소 시작 대기 시간 (마지막 피격 후)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float StaggerDecayDelay = 3.f;
 
	
	// State
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossPhase CurrentPhase = EBossPhase::Phase1_Probe;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossCombatState CurrentCombatState = EBossCombatState::Idle;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	bool bSuperArmor = false;
 
	// 마지막으로 경직 대미지를 받은 시각
	float LastStaggerHitTime = 0.f;
 
	
	// 내부 메서드
	
 
	// HP 변화에 따른 페이즈 전환 체크
	void CheckPhaseTransition();
 
	// 경직 게이지 자연 감소
	void UpdateStaggerDecay(float DeltaTime);
 
	// 사망 처리
	UFUNCTION()
	void HandleDeath();
};