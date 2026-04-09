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

    if (BackgroundImage)
    {
        BackgroundImage->SetVisibility(ESlateVisibility::Hidden);
        BackgroundImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
    }
    if (CutsceneText)
    {
        CutsceneText->SetText(FText::GetEmpty());
        CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0)));
    }
    if (CutsceneMediaPlayer)
    {
        CutsceneMediaPlayer->Close();
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
    bFadingIn = true;
    bFadingOut = false;
    FadeAlpha = 0.f;

    // 배경 숨기고 이전 영상 닫기
    if (BackgroundImage)
    {
        BackgroundImage->SetVisibility(ESlateVisibility::Hidden);
        BackgroundImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
    }
    if (CutsceneMediaPlayer)
        CutsceneMediaPlayer->Close();

    // 텍스트 설정 (투명하게)
    if (CutsceneText)
    {
        CutsceneText->SetText(Step.DisplayText);
        CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0)));
    }

    // 영상이 있으면 미리 로드 (아직 안 보임)
    if (Step.MediaIndex >= 0 && MediaSources.IsValidIndex(Step.MediaIndex) && CutsceneMediaPlayer)
    {
        CutsceneMediaPlayer->OpenSource(MediaSources[Step.MediaIndex]);
    }
}

void UCutsceneWidget::CutsceneTick()
{
    if (!bCutsceneActive || !Steps.IsValidIndex(CurrentStep)) return;

    const FCutsceneStep& Step = Steps[CurrentStep];
    StepTimer += 0.016f;

    float FadeInDuration = 1.0f;
    float FadeOutDuration = 1.0f;
    float FadeOutStart = Step.Duration - FadeOutDuration;

    // 알파 계산
    float Alpha = 1.f;
    if (StepTimer < FadeInDuration)
    {
        // 페이드 인
        Alpha = StepTimer / FadeInDuration;
    }
    else if (StepTimer > FadeOutStart)
    {
        // 페이드 아웃
        Alpha = 1.f - (StepTimer - FadeOutStart) / FadeOutDuration;
    }
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    // 텍스트 알파 적용
    if (CutsceneText)
        CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, Alpha)));

    // 영상 배경 알파 적용
    if (Step.MediaIndex >= 0)
    {
        // 페이드 인 0.5초 지나면 영상 재생 시작
        if (StepTimer >= 0.5f && bFadingIn)
        {
            bFadingIn = false;
            if (CutsceneMediaPlayer)
                CutsceneMediaPlayer->Play();
            if (BackgroundImage)
                BackgroundImage->SetVisibility(ESlateVisibility::Visible);
        }

        if (BackgroundImage && !bFadingIn)
        {
            float BgAlpha = FMath::Clamp((StepTimer - 0.5f) / 0.5f, 0.f, 1.f);
            if (StepTimer > FadeOutStart)
                BgAlpha = FMath::Clamp(1.f - (StepTimer - FadeOutStart) / FadeOutDuration, 0.f, 1.f);
            BackgroundImage->SetColorAndOpacity(FLinearColor(1, 1, 1, BgAlpha));
        }
    }
    else
    {
        // 검은 화면 스텝 — 배경 숨김 유지
        if (BackgroundImage)
        {
            BackgroundImage->SetVisibility(ESlateVisibility::Hidden);
            BackgroundImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
        }
    }

    // 다음 스텝
    if (StepTimer >= Step.Duration)
    {
        CurrentStep++;
        if (CurrentStep >= Steps.Num())
        {
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
    if (BackgroundImage)
    {
        BackgroundImage->SetVisibility(ESlateVisibility::Hidden);
        BackgroundImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
    }
    if (CutsceneText)
        CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0)));
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