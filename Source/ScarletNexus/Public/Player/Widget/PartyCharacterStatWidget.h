// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetView.h"
#include "PartyCharacterStatWidget.generated.h"

class UPartyHUDViewModel;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UPartyCharacterStatWidget : public UWidgetView
{
	GENERATED_BODY()

public:
	virtual void InitViewModel(UViewModel* InViewModel) override;

protected:
	UFUNCTION()
	void UpdateHP(int32 Current, float Percent);

	UFUNCTION()
	void UpdateMaxHP(int32 Max);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> pb_PartyCharacterHP;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> txt_PartyCurHP;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> txt_PartyMaxHP;
};
