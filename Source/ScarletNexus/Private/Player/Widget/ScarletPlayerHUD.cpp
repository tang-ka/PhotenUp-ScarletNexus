// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget//ScarletPlayerHUD.h"
#include "Player/Widget/PlayerStatWidget.h"
#include "Player/Widget/PlayerHUDViewModel.h"
#include "Player/Component/PlayerStatsComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void AScarletPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	// 1. Create ViewModel
	HUDViewModel = NewObject<UPlayerHUDViewModel>(this);

	// 2. Create View (Widget)
	if (PlayerStatWidgetClass)
	{
		PlayerStatWidget = CreateWidget<UPlayerStatWidget>(GetWorld(), PlayerStatWidgetClass);
		if (PlayerStatWidget)
		{
			PlayerStatWidget->AddToViewport();
			PlayerStatWidget->InitViewModel(HUDViewModel);
		}
	}

	// 3. Find Model and Bind
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			if (UPlayerStatsComponent* StatsComp = PlayerPawn->FindComponentByClass<UPlayerStatsComponent>())
			{
				StatsComp->OnHPChanged.AddUObject(HUDViewModel.Get(), &UPlayerHUDViewModel::SetHP);
				StatsComp->OnMaxHPChanged.AddUObject(HUDViewModel.Get(), &UPlayerHUDViewModel::SetMaxHP);
				StatsComp->OnMPChanged.AddUObject(HUDViewModel.Get(), &UPlayerHUDViewModel::SetMP);

				// Initialize current values
				HUDViewModel->SetMaxHP(StatsComp->GetMaxHP());
				HUDViewModel->SetHP(StatsComp->GetCurrentHP());
				
				HUDViewModel->SetMaxMP(StatsComp->GetMaxMP());
				HUDViewModel->SetMP(StatsComp->GetCurrentMP());
			}
		}
	}
}
