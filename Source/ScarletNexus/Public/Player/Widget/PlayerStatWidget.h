// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetView.h"
#include "PlayerStatWidget.generated.h"

class UPlayerHUDViewModel;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UPlayerStatWidget : public UWidgetView
{
	GENERATED_BODY()
	
public:
	virtual void InitViewModel(UViewModel* InViewModel) override;
	
protected:
	UFUNCTION()
	void UpdateHP(int32 Current, float Percent);
	
	UFUNCTION()
	void UpdateMaxHP(int32 Max);
	
	UFUNCTION()
	void UpdateMP(int32 Current, float Percent);

	UFUNCTION()
	void UpdateMaxMP(int32 Max);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> PGBar_PlayerHP;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> PGBar_PlayerMP;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_CurHP;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_MaxHP;
};
