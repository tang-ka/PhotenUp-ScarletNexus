// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ScarletPlayerHUD.generated.h"

class UDamageAmountWidget;
class UPlayerStatWidget;
class UPlayerHUDViewModel;

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API AScarletPlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerStatWidget> PlayerStatWidgetClass;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerStatWidget> PlayerStatWidget;
	
	UPROPERTY(Transient)
	TObjectPtr<UDamageAmountWidget> DamageAmountWidget;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHUDViewModel> HUDViewModel;
};
