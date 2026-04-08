// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "ICombatState.h"
#include "BossTypes.h"
#include "NiagaraSystem.h"
#include "BossCharacterBase.generated.h"
 
class UBossConfigDataAsset;
class UBossAttackCollisionComponent;
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBossPhaseChanged,
	EBossPhase, OldPhase,
	EBossPhase, NewPhase);
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnBossHPChanged,
	float, CurrentHP,
	float, MaxHP,
	float, DamageAmount);
 
UCLASS()
class SCARLETNEXUS_API ABossCharacterBase : public ACharacter,
	public IDamageable, public ICombatState
{
	GENERATED_BODY()
 
public:
	ABossCharacterBase();
 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
 
	UFUNCTION(BlueprintCallable, Category = "Boss")
	EBossPhase GetCurrentPhase() const { return CurrentPhase; }
 
	// IDamageable 구현 (데미지 + HP 조회)
	
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	virtual int GetHP_Implementation() const override;
	virtual float GetHPPercent_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
 
	// ICombatState 구현
	
	virtual bool IsAttacking_Implementation() const override;
	virtual bool HasSuperArmor_Implementation() const override;
 
	
	// 보스 전용
	
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void InitializeWithConfig(UBossConfigDataAsset* Config);
 
	
	// Delegates
	
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossPhaseChanged OnPhaseChanged;
 
	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossHPChanged OnHPChanged;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<UAnimMontage> TeleportVanishMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Teleport")
	TObjectPtr<UAnimMontage> TeleportAppearMontage;
	
	// 텔레포트킥 쿨타임
	UPROPERTY()
	float PostAttackTeleportCooldown = 0.f;
	
	
	// 스폰함수
	void SpawnGhostTrail(float Lifetime = 0.5f);
	
	// 머티리얼
	void StartDissolve(float Duration, bool bDissolveOut);
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss|VFX")
	TObjectPtr<UMaterialInterface> IceSpikeWarningMaterial;
	
	// 나이아가라
	UPROPERTY(EditDefaultsOnly, Category = "Boss|VFX")
	TObjectPtr<UNiagaraSystem> IceSpikeVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss|VFX")
	TObjectPtr<UNiagaraSystem> LightningVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss|VFX")
	TObjectPtr<UNiagaraSystem> ElectricOrbVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss|VFX")
	TObjectPtr<UNiagaraSystem> ElectricOrbHitVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss|VFX")
	TObjectPtr<UNiagaraSystem> IceSpikeWarningVFX;
	
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DissolveMIDs;
    
	float DissolveTimer = 0.f;
	float DissolveDuration = 0.f;
	bool bDissolving = false;
	bool bDissolveOut = true;  // true = 사라짐, false = 나타남

 
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Config")
	TObjectPtr<UBossConfigDataAsset> BossConfig;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float MaxHPValue = 10000.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHPValue = 10000.f;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossCombatState CurrentCombatState = EBossCombatState::Idle;
	
	// 공격 콜리전 컴포넌트 (본에 부착)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Combat")
	TObjectPtr<UBossAttackCollisionComponent> RightFootCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Combat")
	TObjectPtr<UBossAttackCollisionComponent> LeftFootCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Combat")
	TObjectPtr<UBossAttackCollisionComponent> RightHandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Combat")
	TObjectPtr<UBossAttackCollisionComponent> LeftHandCollision;
	
	// 히트 리액션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Boss|HitReaction")
	TObjectPtr<UAnimMontage> HitReaction_Front;
	UPROPERTY(EditDefaultsOnly, Category = "Boss|HitReaction")
	TObjectPtr<UAnimMontage> HitReaction_Back;
	UPROPERTY(EditDefaultsOnly, Category = "Boss|HitReaction")
	TObjectPtr<UAnimMontage> HitReaction_Left;
	UPROPERTY(EditDefaultsOnly, Category = "Boss|HitReaction")
	TObjectPtr<UAnimMontage> HitReaction_Right;
	

	
	
	
	void PlayDirectionalHitReaction(AActor* DamageCauser);
 
	void CheckPhaseTransition();
 
	UFUNCTION()
	void HandleDeath();
};