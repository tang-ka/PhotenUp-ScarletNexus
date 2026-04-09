// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"


UCLASS()
class UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_GameStart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Exit;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnGameStart();

	UFUNCTION()
	void OnExitGame();
};
