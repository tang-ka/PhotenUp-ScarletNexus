// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/AttackType.h"
#include "Components/ActorComponent.h"
#include "ComboComponent.generated.h"

class UComboAttackDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnComboExecuted,
	const UComboAttackDataAsset*, ExecutedAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboReset);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UComboComponent();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool TryExecuteCombo(EAttackType InputType);
	
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();
	
	UFUNCTION(BlueprintPure, Category = "Combo")
	bool IsInCombo() const {  return CurrentAttack != nullptr; }
	
	UFUNCTION(BlueprintPure, Category = "Combo")
	int32 GetCurrentComboIndex() const { return CurComboIndex; }

	UFUNCTION(BlueprintPure, Category = "Combo")
	const UComboAttackDataAsset* GetCurrentAttack() const { return CurrentAttack; }
	
private:
	UComboAttackDataAsset* LoadAttackDataAsset(const TSoftObjectPtr<UComboAttackDataAsset>& SoftPtr) const;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Combo")
	FOnComboExecuted OnComboExecuted;
	
	UPROPERTY(BlueprintAssignable, Category = "Combo")
	FOnComboReset OnComboReset;
	
private:
	UPROPERTY(EditAnywhere, Category = "Combo | Settings")
	TMap<EAttackType, TSoftObjectPtr<UComboAttackDataAsset>> RootAttackMap;
	
	UPROPERTY()
	TObjectPtr<UComboAttackDataAsset> CurrentAttack = nullptr;

	int32 CurComboIndex = 0;
	
	UPROPERTY()
	FTimerHandle ComboResetTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Combo")
	float ComboResetTime = 1.5f; // 콤보 입력 간 최대 허용 시간
};
