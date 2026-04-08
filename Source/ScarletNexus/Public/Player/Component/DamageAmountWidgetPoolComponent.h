// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageAmountWidgetPoolComponent.generated.h"


struct FDamageWidgetData;
class UDamageAmountWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UDamageAmountWidgetPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamageAmountWidgetPoolComponent();
	virtual void BeginPlay() override;
	
public:
	void ShowDamageNumber(const FDamageWidgetData& InData);
	
private:
	void InitializePool();
	UDamageAmountWidget* GetFromPool();
	void ReturnToPool(UDamageAmountWidget* Widget);
	void CreateNewWidgetToPool();

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	TSubclassOf<UDamageAmountWidget> DamageWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pool")
	int32 PoolSize = 10;

	UPROPERTY()
	TArray<TObjectPtr<UDamageAmountWidget>> AvailablePool;

	UPROPERTY()
	TArray<TObjectPtr<UDamageAmountWidget>> ActivePool;
};
