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
        FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, 1.f));
    if (CutsceneText)
        CutsceneText->SetText(FText::GetEmpty());
    if (CutsceneMediaPlayer)
        CutsceneMediaPlayer->Close();
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

    // 페이드 완전 검정
    if (FadeImage)
        FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, 1.f));

    // 배경 숨기기
    if (BackgroundImage)
        BackgroundImage->SetVisibility(ESlateVisibility::Hidden);

    if (CutsceneMediaPlayer)
        CutsceneMediaPlayer->Close();

    // 텍스트 설정
    if (CutsceneText)
    {
        CutsceneText->SetText(Step.DisplayText);
        CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0)));
    }

    // 영상 있으면 로드 + 배경 보이기
    if (Step.MediaIndex >= 0 && MediaSources.IsValidIndex(Step.MediaIndex) && CutsceneMediaPlayer)
    {
        CutsceneMediaPlayer->OpenSource(MediaSources[Step.MediaIndex]);
        if (BackgroundImage)
            BackgroundImage->SetVisibility(ESlateVisibility::Visible);
    }
}

void UCutsceneWidget::CutsceneTick()
{
    if (!bCutsceneActive || !Steps.IsValidIndex(CurrentStep)) return;

    const FCutsceneStep& Step = Steps[CurrentStep];
    StepTimer += 0.016f;

    // 페이드 인 (1초)
    if (bFadingIn)
    {
        FadeAlpha -= 0.016f / 1.0f;
        if (FadeAlpha <= 0.f)
        {
            FadeAlpha = 0.f;
            bFadingIn = false;
        }

        if (FadeImage)
            FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, FMath::Clamp(FadeAlpha, 0.f, 1.f)));

        float TextAlpha = 1.f - FadeAlpha;
        if (CutsceneText)
            CutsceneText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, TextAlpha)));

        return;
    }

    // 페이드 아웃 (마지막 1초)
    float TimeLeft = Step.Duration - StepTimer;
    if (TimeLeft <= 1.0f && !bFadingOut)
    {
        bFadingOut = true;
        FadeAlpha = 0.f;
    }

    if (bFadingOut)
    {
        FadeAlpha += 0.016f / 1.0f;
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

    FName LevelName = NextLevelName;
    UWorld* World = GetWorld();

    FTimerHandle DelayHandle;
    World->GetTimerManager().SetTimer(
        DelayHandle,
        [World, LevelName]()
        {
            UGameplayStatics::OpenLevel(World, LevelName);
        },
        1.0f,
        false
    );
}