// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/CutsceneWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UCutsceneWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (FadeImage)
    {
        FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, 1.f));
    }
    if (CutsceneText)
    {
        CutsceneText->SetText(FText::GetEmpty());
    }
}

void UCutsceneWidget::StartCutscene()
{
    if (Steps.Num() == 0) return;

    CurrentStep = 0;
    bCutsceneActive = true;
    ShowStep(0);

    GetWorld()->GetTimerManager().SetTimer(
        CutsceneTickHandle, this, &UCutsceneWidget::CutsceneTick, 0.016f, true);
}

void UCutsceneWidget::ShowStep(int32 Index)
{
    if (!Steps.IsValidIndex(Index)) return;

    const FCutsceneStep& Step = Steps[Index];
    StepTimer = 0.f;
    FadeAlpha = 1.f;
    bFadingIn = true;
    bFadingOut = false;

    UE_LOG(LogTemp, Warning, TEXT("[Cutscene] Step %d, MediaIndex: %d, Text: %s"), 
        Index, Step.MediaIndex, *Step.DisplayText.ToString());

    if (CutsceneText)
    {
        CutsceneText->SetText(Step.DisplayText);
        CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0)));
    }

    if (Step.MediaIndex >= 0 && MediaSources.IsValidIndex(Step.MediaIndex) && CutsceneMediaPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Cutscene] 영상 재생: %s"), *MediaSources[Step.MediaIndex]->GetName());
        CutsceneMediaPlayer->OpenSource(MediaSources[Step.MediaIndex]);
        CutsceneMediaPlayer->Play();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Cutscene] 검은 화면"));
        if (CutsceneMediaPlayer)
            CutsceneMediaPlayer->Close();
    }
}

void UCutsceneWidget::CutsceneTick()
{
    if (!bCutsceneActive || !Steps.IsValidIndex(CurrentStep)) return;

    const FCutsceneStep& Step = Steps[CurrentStep];
    StepTimer += 0.016f;

    // 페이드 인 (0.5초)
    if (bFadingIn)
    {
        FadeAlpha -= 0.016f / 0.5f;
        if (FadeAlpha <= 0.f)
        {
            FadeAlpha = 0.f;
            bFadingIn = false;
        }

        if (FadeImage)
            FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, FMath::Clamp(FadeAlpha, 0.f, 1.f)));

        // 텍스트도 같이 페이드 인
        float TextAlpha = 1.f - FadeAlpha;
        if (CutsceneText)
            CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, TextAlpha)));

        return;
    }

    // 페이드 아웃 (마지막 0.5초)
    float TimeLeft = Step.Duration - StepTimer;
    if (TimeLeft <= 0.5f && !bFadingOut)
    {
        bFadingOut = true;
        FadeAlpha = 0.f;
    }

    if (bFadingOut)
    {
        FadeAlpha += 0.016f / 0.5f;
        if (FadeImage)
            FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, FMath::Clamp(FadeAlpha, 0.f, 1.f)));

        float TextAlpha = 1.f - FadeAlpha;
        if (CutsceneText)
            CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, FMath::Clamp(TextAlpha, 0.f, 1.f))));
    }

    // 다음 스텝
    if (StepTimer >= Step.Duration)
    {
        CurrentStep++;
        if (CurrentStep >= Steps.Num())
        {
            // 컷신 끝 → 레벨 전환
            GetWorld()->GetTimerManager().ClearTimer(CutsceneTickHandle);
            bCutsceneActive = false;
            TransitionToNextLevel();
        }
        else
        {
            ShowStep(CurrentStep);
        }
    }
}

void UCutsceneWidget::FadeToBlack()
{
    if (FadeImage)
        FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, 1.f));
}

void UCutsceneWidget::TransitionToNextLevel()
{
    FadeToBlack();

    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DelayHandle,
        [this]()
        {
            UGameplayStatics::OpenLevel(this, NextLevelName);
        },
        1.0f,
        false
    );
}
