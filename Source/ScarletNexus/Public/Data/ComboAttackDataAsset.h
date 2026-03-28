// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComboAttackDataAsset.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None			UMETA(DisplayName = "None"),
	BasicAttack		UMETA(DisplayName = "Basic Attack"),
	PsychicAttack	UMETA(DisplayName = "Psychic Attack"),
	BackStepAttack	UMETA(DisplayName = "Back Step Attack")
};

USTRUCT(BlueprintType)
struct FAttackBalanceData : public FTableRowBase
{
	GENERATED_BODY()
	
	// ===== 데미지 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float CriticalMultiplier = 1.5f;
	
	// ===== 자원 소모 및 획득 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	float StaminaCost = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	float PsychicGaugeGain = 2.f;
	
	// ===== 타이밍 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float ComboWindowStart = 0.3f; // 몽타주 시작 후 윈도우 열리는 시간
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float ComboWindowDuration = 0.2f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float CooldownTime = 0.0f;
};

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UComboAttackDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// ===== 식별 정보 =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName AttackID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EAttackType AttackType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity");
	FText AttackDisplayName = FText::FromString("Unnamed Attack");
	
	// ===== 애니메이션 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MontagePlayRate = 1.0f;
	
	// ===== 콤보 연결 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TMap<EAttackType, TSoftObjectPtr<UComboAttackDataAsset>> NextComboMap;
	
	// ===== VFX/SFX =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<UParticleSystem> HitEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<USoundBase> AttackSound;
	
	// ===== 밸런스 데이터 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (RowType = "AttackBalanceData"))
	FDataTableRowHandle BalanceDataHandle;
	
	UFUNCTION(BlueprintCallable, Category = "Balance")
	FAttackBalanceData GetBalanceData() const
	{
		if (const FAttackBalanceData* Data = BalanceDataHandle.GetRow<FAttackBalanceData>(TEXT("")))
		{
			return *Data;
		}
		return FAttackBalanceData(); // 기본값 반환
	}
};