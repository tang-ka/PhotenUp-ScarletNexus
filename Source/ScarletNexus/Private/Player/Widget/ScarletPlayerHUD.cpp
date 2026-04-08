// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget//ScarletPlayerHUD.h"
#include "Player/Widget/PlayerStatWidget.h"
#include "Player/Widget/PlayerHUDViewModel.h"
#include "Player/Widget/PartyCharacterStatWidget.h"
#include "Player/Widget/PartyHUDViewModel.h"
#include "Player/Component/PlayerStatsComponent.h"
#include "Player/Component/PartyHandlerComponent.h"
#include "PartyAI/PartyMemberBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void AScarletPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	// 1. Create ViewModel
	PlayerStatViewModel = NewObject<UPlayerHUDViewModel>(this);

	// 2. Create View (Widget)
	if (PlayerStatWidgetClass)
	{
		PlayerStatWidget = CreateWidget<UPlayerStatWidget>(GetWorld(), PlayerStatWidgetClass);
		if (PlayerStatWidget)
		{
			PlayerStatWidget->AddToViewport();
			PlayerStatWidget->InitViewModel(PlayerStatViewModel);
		}
	}
	// 4. 파티 위젯 생성 (초기에는 Hidden)
	if (PartyCharacterWidgetClass)
	{
		PartyCharacterWidget = CreateWidget<UPartyCharacterStatWidget>(GetWorld(), PartyCharacterWidgetClass);
		if (PartyCharacterWidget)
		{
			PartyCharacterWidget->AddToViewport();
			PartyCharacterWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 3. Find Model and Bind
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			// 플레이어 스탯 바인딩
			if (UPlayerStatsComponent* StatsComp = PlayerPawn->FindComponentByClass<UPlayerStatsComponent>())
			{
				StatsComp->OnHPChanged.AddUObject(PlayerStatViewModel.Get(), &UPlayerHUDViewModel::SetHP);
				StatsComp->OnMaxHPChanged.AddUObject(PlayerStatViewModel.Get(), &UPlayerHUDViewModel::SetMaxHP);
				StatsComp->OnMPChanged.AddUObject(PlayerStatViewModel.Get(), &UPlayerHUDViewModel::SetMP);

				// Initialize current values
				PlayerStatViewModel->SetMaxHP(StatsComp->GetMaxHP());
				PlayerStatViewModel->SetHP(StatsComp->GetCurrentHP());
				
				PlayerStatViewModel->SetMaxMP(StatsComp->GetMaxMP());
				PlayerStatViewModel->SetMP(StatsComp->GetCurrentMP());
			}
			
			// 5. PartyHandlerComponent 델리게이트 바인딩
			if (UPartyHandlerComponent* PartyComp = PlayerPawn->FindComponentByClass<UPartyHandlerComponent>())
			{
				PartyComp->OnPartyMemberAdded.AddUObject(this, &AScarletPlayerHUD::ShowPartyWidget);
				PartyComp->OnPartyMemberRemoved.AddUObject(this, &AScarletPlayerHUD::HidePartyWidget);
			}
		}
	}
}

void AScarletPlayerHUD::ShowPartyWidget(APartyMemberBase* PartyMember)
{
	if (!PartyMember || !PartyCharacterWidget)
	{
		return;
	}

	// 이전 파티원 델리게이트 해제
	if (CurrentPartyMember)
	{
		CurrentPartyMember->OnHPChanged.RemoveAll(this);
	}
	CurrentPartyMember = PartyMember;

	// ViewModel 생성 (재사용 or 신규)
	if (!PartyViewModel)
	{
		PartyViewModel = NewObject<UPartyHUDViewModel>(this);
		PartyCharacterWidget->InitViewModel(PartyViewModel);
	}

	// 초기값 설정
	PartyViewModel->SetMaxHP(PartyMember->MaxHP);
	PartyViewModel->SetHP(PartyMember->CurrHP);

	// HP 변경 바인딩
	PartyMember->OnHPChanged.AddUObject(this, &AScarletPlayerHUD::OnPartyMemberHPChanged);

	// 위젯 표시
	PartyCharacterWidget->SetVisibility(ESlateVisibility::Visible);
}

void AScarletPlayerHUD::HidePartyWidget(APartyMemberBase* PartyMember)
{
	// 델리게이트 해제
	if (CurrentPartyMember)
	{
		CurrentPartyMember->OnHPChanged.RemoveAll(this);
		CurrentPartyMember = nullptr;
	}

	if (PartyCharacterWidget)
	{
		PartyCharacterWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AScarletPlayerHUD::OnPartyMemberHPChanged(int32 CurrHP, int32 MaxHP)
{
	if (!PartyViewModel)
	{
		return;
	}
	PartyViewModel->SetMaxHP(MaxHP);
	PartyViewModel->SetHP(CurrHP);
}

