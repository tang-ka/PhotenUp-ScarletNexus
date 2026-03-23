// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"


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
	float GetHPPercentage() const { return static_cast<float>(CurrentHP) / MaxHP; }
	
	UFUNCTION(BlueprintCallable)
	void Heal(int32 HealAmount) { SetCurrentHP(CurrentHP + HealAmount); }
	
	UFUNCTION(BlueprintCallable)
	void ReceiveDamage(int32 DamageAmount) { SetCurrentHP(CurrentHP - DamageAmount); }
	
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
	void SetCurrentHP(int32 NewHP) { CurrentHP = FMath::Clamp(NewHP, 0, MaxHP); }
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentMP(int32 NewMP) { CurrentMP = FMath::Clamp(NewMP, 0, MaxMP); }
#pragma endregion 
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 MaxHP{100};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 CurrentHP{100};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 MaxMP{100};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats", meta=(AllowPrivateAccess=true))
	int32 CurrentMP{100};
};
