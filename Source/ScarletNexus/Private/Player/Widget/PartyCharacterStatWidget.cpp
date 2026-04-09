// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Widget/PartyCharacterStatWidget.h"
#include "Player/Widget/PartyHUDViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPartyCharacterStatWidget::InitViewModel(UViewModel* InViewModel)
{
	Super::InitViewModel(InViewModel);

	if (UPartyHUDViewModel* VM = Cast<UPartyHUDViewModel>(OwnerViewModel))
	{
		VM->OnHPUpdated.AddUObject(this, &UPartyCharacterStatWidget::UpdateHP);
		VM->OnMaxHPUpdated.AddUObject(this, &UPartyCharacterStatWidget::UpdateMaxHP);
	}
}

void UPartyCharacterStatWidget::UpdateHP(int32 Current, float Percent)
{
	if (pb_PartyCharacterHP)
	{
		pb_PartyCharacterHP->SetPercent(Percent);
	}
	if (txt_PartyCurHP)
	{
		txt_PartyCurHP->SetText(FText::AsNumber(Current));
	}
}

void UPartyCharacterStatWidget::UpdateMaxHP(int32 Max)
{
	if (txt_PartyMaxHP)
	{
		txt_PartyMaxHP->SetText(FText::AsNumber(Max));
	}
}
