// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CutsceneWidget.generated.h"

class UImage;
class UTextBlock;
class UMediaPlayer;
class UMediaSource;

USTRUCT(BlueprintType)
struct FCutsceneStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MediaIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 3.0f;
};

UCLASS()
class SCARLETNEXUS_API UCutsceneWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FadeImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CutsceneText;

	// BP에서 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene")
	TArray<FCutsceneStep> Steps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene")
	TArray<TObjectPtr<UMediaSource>> MediaSources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene")
	TObjectPtr<UMediaPlayer> CutsceneMediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene")
	FName NextLevelName;

	UFUNCTION(BlueprintCallable)
	void StartCutscene();

protected:
	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	int32 CurrentStep = 0;
	float StepTimer = 0.f;
	float FadeAlpha = 0.f;
	bool bFadingIn = true;
	bool bFadingOut = false;
	bool bCutsceneActive = false;

	FTimerHandle CutsceneTickHandle;

	void CutsceneTick();
	void ShowStep(int32 Index);
	void FadeToBlack();
	void TransitionToNextLevel();
};