// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/StartMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_GameStart->OnClicked.AddDynamic(this, &UStartMenuWidget::OnGameStart);
	Button_Exit->OnClicked.AddDynamic(this, &UStartMenuWidget::OnExitGame);
}

void UStartMenuWidget::OnGameStart()
{
	UGameplayStatics::OpenLevel(this, FName("BossLevel"));
}

void UStartMenuWidget::OnExitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}