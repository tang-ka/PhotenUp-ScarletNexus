// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerStatsComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UFUNCTION(BlueprintCallable)
	float GetHPPercentage() const;

	UFUNCTION(BlueprintCallable)
	void HealAmount(int32 HealAmount);

	UFUNCTION(BlueprintCallable)
	void HealPercent(float HealPercent);

	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(int32 DamageAmount);

	UFUNCTION(BlueprintCallable)
	bool IsDead() const { return CurrentHP <= 0; }

public:
	FOnStatChangedDelegate OnHPChanged;
	FOnStatChangedDelegate OnMaxHPChanged;
	FOnStatChangedDelegate OnMPChanged;
	FOnDeathDelegate OnDeath;

#pragma region Getters & Setters
	UFUNCTION(BlueprintCallable)
	int32 GetMaxHP() const { return MaxHP; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentHP() const { return CurrentHP; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetMaxMP() const { return MaxMP; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentMP() const { return CurrentMP; }
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentHP(int32 NewHP);

	UFUNCTION(BlueprintCallable)
	void SetMaxHP(int32 NewMaxHP);

	UFUNCTION(BlueprintCallable)
	void SetCurrentMP(int32 NewMP);

	UFUNCTION(BlueprintCallable)
	void SetMaxMP(int32 NewMaxMP);
#pragma endregion 
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 MaxHP{800};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 CurrentHP{0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 MaxMP{100};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 CurrentMP{0};
};
