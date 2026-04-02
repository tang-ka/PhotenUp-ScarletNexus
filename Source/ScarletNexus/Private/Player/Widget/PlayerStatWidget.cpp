// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget/PlayerStatWidget.h"

#include "Player/Widget/PlayerHUDViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerStatWidget::InitViewModel(UViewModel* InViewModel)
{
	Super::InitViewModel(InViewModel);
	
	if (UPlayerHUDViewModel* HUDViewModel = Cast<UPlayerHUDViewModel>(OwnerViewModel))
	{
		HUDViewModel->OnHPUpdated.AddUObject(this, &UPlayerStatWidget::UpdateHP);
		HUDViewModel->OnMaxHPUpdated.AddUObject(this, &UPlayerStatWidget::UpdateMaxHP);
		HUDViewModel->OnMPUpdated.AddUObject(this, &UPlayerStatWidget::UpdateMP);
	}
}

void UPlayerStatWidget::UpdateHP(int32 Current, float Percent)
{
	if (PGBar_PlayerHP)
	{
		PGBar_PlayerHP->SetPercent(Percent);
	}

	if (Txt_CurHP)
	{
		Txt_CurHP->SetText(FText::AsNumber(Current));
	}
}

void UPlayerStatWidget::UpdateMaxHP(int32 Max)
{
	if (Txt_MaxHP)
	{
		Txt_MaxHP->SetText(FText::AsNumber(Max));
	}
}

void UPlayerStatWidget::UpdateMP(int32 Current, float Percent)
{
	if (PGBar_PlayerMP)
	{
		PGBar_PlayerMP->SetPercent(Percent);
	}
}

void UPlayerStatWidget::UpdateMaxMP(int32 Max)
{
}
