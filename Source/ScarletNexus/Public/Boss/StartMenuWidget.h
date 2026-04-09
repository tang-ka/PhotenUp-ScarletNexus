// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class SCARLETNEXUS_API UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_GameStart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Exit;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FadeImage;

	FTimerHandle FadeTimerHandle;
	float FadeAlpha = 0.f;

	void FadeTick();
	
	bool bGameStartSelected = false;
	bool bExitSelected = false;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_GameStart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Exit;
	
	FTimerHandle ColorFadeHandle;
	float ColorFadeAlpha = 0.f;
	bool bFadingGameStart = false;
	bool bFadingExit = false;

	void ColorFadeTick();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnGameStart();

	UFUNCTION()
	void OnExitGame();
};
