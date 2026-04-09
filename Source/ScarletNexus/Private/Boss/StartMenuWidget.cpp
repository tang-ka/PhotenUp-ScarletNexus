// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/StartMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"




void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_GameStart->OnClicked.AddDynamic(this, &UStartMenuWidget::OnGameStart);
	Button_Exit->OnClicked.AddDynamic(this, &UStartMenuWidget::OnExitGame);
}



void UStartMenuWidget::OnGameStart()
{
    if (bGameStartSelected)
    {
        FadeAlpha = 0.f;
        GetWorld()->GetTimerManager().SetTimer(
            FadeTimerHandle, this, &UStartMenuWidget::FadeTick, 0.016f, true);
        return;
    }

    bGameStartSelected = true;
    bExitSelected = false;
    bFadingGameStart = true;
    bFadingExit = false;
    ColorFadeAlpha = 0.f;
    
    // Exit은 즉시 흰색 복원
    Text_Exit->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    Text_Exit->SetRenderScale(FVector2D(1.0f, 1.0f));
    Text_GameStart->SetRenderScale(FVector2D(1.1f, 1.1f));

    GetWorld()->GetTimerManager().SetTimer(
        ColorFadeHandle, this, &UStartMenuWidget::ColorFadeTick, 0.016f, true);
}

void UStartMenuWidget::FadeTick()
{
    FadeAlpha += 0.016f / 1.5f;  // 1.5초에 걸쳐 페이드
    
    if (FadeImage)
    {
        FadeImage->SetColorAndOpacity(FLinearColor(0, 0, 0, FMath::Clamp(FadeAlpha, 0.f, 1.f)));
    }

    if (FadeAlpha >= 1.f)
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
        UGameplayStatics::OpenLevel(this, FName("CutsceneLevel1"));
    }
}

void UStartMenuWidget::OnExitGame()
{
    if (bExitSelected)
    {
        UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
        return;
    }

    bExitSelected = true;
    bGameStartSelected = false;
    bFadingExit = true;
    bFadingGameStart = false;
    ColorFadeAlpha = 0.f;

    Text_GameStart->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    Text_GameStart->SetRenderScale(FVector2D(1.0f, 1.0f));
    Text_Exit->SetRenderScale(FVector2D(1.1f, 1.1f));

    GetWorld()->GetTimerManager().SetTimer(
        ColorFadeHandle, this, &UStartMenuWidget::ColorFadeTick, 0.016f, true);
}

void UStartMenuWidget::ColorFadeTick()
{
    ColorFadeAlpha += 0.016f / 0.3f;  // 0.3초에 걸쳐 변화

    if (ColorFadeAlpha >= 1.f)
    {
        ColorFadeAlpha = 1.f;
        GetWorld()->GetTimerManager().ClearTimer(ColorFadeHandle);
    }

    FLinearColor Color = FMath::Lerp(FLinearColor::White, FLinearColor::Red, ColorFadeAlpha);

    if (bFadingGameStart)
    {
        Text_GameStart->SetColorAndOpacity(FSlateColor(Color));
    }
    else if (bFadingExit)
    {
        Text_Exit->SetColorAndOpacity(FSlateColor(Color));
    }
}