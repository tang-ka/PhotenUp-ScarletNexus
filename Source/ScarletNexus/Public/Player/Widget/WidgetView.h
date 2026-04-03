// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetView.generated.h"

class UViewModel;
/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UWidgetView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitViewModel(UViewModel* ViewModel) { OwnerViewModel = ViewModel; }
	
protected:
	UPROPERTY()
	TObjectPtr<UViewModel> OwnerViewModel;
};
