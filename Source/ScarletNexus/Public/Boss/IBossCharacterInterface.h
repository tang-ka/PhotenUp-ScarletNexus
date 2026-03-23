// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossTypes.h"
#include "IBossCharacterInterface.generated.h"
 
UINTERFACE(MinimalAPI, BlueprintType)
class UBossCharacterInterface : public UInterface
{
	GENERATED_BODY()
};
 
class SCARLETNEXUS_API IBossCharacterInterface
{
	GENERATED_BODY()
 
public:
	// HP 관련
 
	// 현재 HP 비율 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	float GetHPRatio() const;
 
	// 절대 HP 값
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	float GetCurrentHP() const;
 
	// 대미지 적용
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	void ApplyDamage(float DamageAmount, AActor* DamageCauser);
 
	// 경직 (Stagger) 관련
 
	// 경직 게이지 비율 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	float GetStaggerRatio() const;
 
	// 경직 대미지 적용
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	void ApplyStaggerDamage(float StaggerAmount);
 
	// 경직 상태인지 여부
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	bool IsStaggered() const;
 
	// 전투 상태
 
	// 현재 전투 상태
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	EBossCombatState GetCombatState() const;
 
	// 공격 중인지
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	bool IsAttacking() const;
 
	// 슈퍼아머(피격 무시) 활성 여부
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	bool HasSuperArmor() const;
 
	// 초기화
	
 
	// DataAsset으로 초기 설정
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Boss")
	void InitializeWithConfig(UBossConfigDataAsset* Config);
};
