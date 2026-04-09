// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget//ScarletPlayerHUD.h"
#include "Player/Widget/PlayerStatWidget.h"
#include "Player/Widget/PlayerHUDViewModel.h"
#include "Player/Widget/PartyCharacterStatWidget.h"
#include "Player/Widget/PartyHUDViewModel.h"
#include "Player/Widget/BossHUDViewModel.h"
#include "Boss/BossHUDWidget.h"
#include "Boss/BossCharacterBase.h"
#include "Player/Component/PlayerStatsComponent.h"
#include "Player/Component/PartyHandlerComponent.h"
#include "PartyAI/PartyMemberBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void AScarletPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	// ── 플레이어 스탯 ─────────────────────────────────────────
	PlayerStatViewModel = NewObject<UPlayerHUDViewModel>(this);

	if (PlayerStatWidgetClass)
	{
		PlayerStatWidget = CreateWidget<UPlayerStatWidget>(GetWorld(), PlayerStatWidgetClass);
		if (PlayerStatWidget)
		{
			PlayerStatWidget->AddToViewport();
			PlayerStatWidget->InitViewModel(PlayerStatViewModel);
		}
	}

	// ── 파티 위젯 (초기 Hidden) ───────────────────────────────
	if (PartyCharacterWidgetClass)
	{
		PartyCharacterStatWidget = CreateWidget<UPartyCharacterStatWidget>(GetWorld(), PartyCharacterWidgetClass);
		if (PartyCharacterStatWidget)
		{
			PartyCharacterStatWidget->AddToViewport();
			PartyCharacterStatWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// ── 보스 HUD ─────────────────────────────────────────────
	// 1) ViewModel 생성
	BossHUDViewModel = NewObject<UBossHUDViewModel>(this);

	// 2) 위젯 생성 (초기 Hidden)
	if (BossHUDWidgetClass)
	{
		BossHUDWidget = CreateWidget<UBossHUDWidget>(GetWorld(), BossHUDWidgetClass);
		if (BossHUDWidget)
		{
			BossHUDWidget->AddToViewport();
			BossHUDWidget->SetVisibility(ESlateVisibility::Hidden);
			// 3) ViewModel → 위젯 델리게이트 바인딩
			BossHUDWidget->InitViewModel(BossHUDViewModel);
		}
	}

	// ── 플레이어 & 파티 델리게이트 바인딩 ────────────────────
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			if (UPlayerStatsComponent* StatsComp = PlayerPawn->FindComponentByClass<UPlayerStatsComponent>())
			{
				StatsComp->OnHPChanged.AddUObject(PlayerStatViewModel.Get(), &UPlayerHUDViewModel::SetHP);
				StatsComp->OnMaxHPChanged.AddUObject(PlayerStatViewModel.Get(), &UPlayerHUDViewModel::SetMaxHP);
				StatsComp->OnMPChanged.AddUObject(PlayerStatViewModel.Get(), &UPlayerHUDViewModel::SetMP);

				PlayerStatViewModel->SetMaxHP(StatsComp->GetMaxHP());
				PlayerStatViewModel->SetHP(StatsComp->GetCurrentHP());
				PlayerStatViewModel->SetMaxMP(StatsComp->GetMaxMP());
				PlayerStatViewModel->SetMP(StatsComp->GetCurrentMP());
			}

			if (UPartyHandlerComponent* PartyComp = PlayerPawn->FindComponentByClass<UPartyHandlerComponent>())
			{
				PartyComp->OnPartyMemberAdded.AddUObject(this, &AScarletPlayerHUD::ShowPartyWidget);
				PartyComp->OnPartyMemberRemoved.AddUObject(this, &AScarletPlayerHUD::HidePartyWidget);
			}
		}
	}

	// ── 보스 자동 탐색 (레벨에 배치된 경우) ──────────────────
	// 보스가 동적 스폰되는 경우에는 RegisterBoss() 를 직접 호출하세요.
	if (ABossCharacterBase* FoundBoss = Cast<ABossCharacterBase>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacterBase::StaticClass())))
	{
		RegisterBoss(FoundBoss);
	}
}

void AScarletPlayerHUD::RegisterBoss(ABossCharacterBase* BossCharacter)
{
	if (!BossCharacter || !BossHUDViewModel) return;

	// 이전 보스 델리게이트 해제
	if (CurrentBoss)
	{
		CurrentBoss->OnHPChanged.RemoveDynamic(BossHUDViewModel.Get(), &UBossHUDViewModel::HandleBossHPChanged);
		CurrentBoss->OnPhaseChanged.RemoveDynamic(BossHUDViewModel.Get(), &UBossHUDViewModel::HandleBossPhaseChanged);
	}
	CurrentBoss = BossCharacter;

	// 초기 상태 밀어 넣기 (위젯 ProgressBar 즉시 반영)
	BossHUDViewModel->SetInitialState(
		static_cast<float>(BossCharacter->GetHP_Implementation()),
		BossCharacter->GetMaxHP());

	// 보스 → ViewModel 델리게이트 체인 연결
	BossCharacter->OnHPChanged.AddDynamic(BossHUDViewModel.Get(), &UBossHUDViewModel::HandleBossHPChanged);
	BossCharacter->OnPhaseChanged.AddDynamic(BossHUDViewModel.Get(), &UBossHUDViewModel::HandleBossPhaseChanged);

	// 위젯 표시
	if (BossHUDWidget)
	{
		BossHUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AScarletPlayerHUD::ShowPartyWidget(APartyMemberBase* PartyMember)
{
	if (!PartyMember || !PartyCharacterStatWidget)
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
	if (!PartyCharacterStatViewModel)
	{
		PartyCharacterStatViewModel = NewObject<UPartyHUDViewModel>(this);
		PartyCharacterStatWidget->InitViewModel(PartyCharacterStatViewModel);
	}

	// 초기값 설정
	PartyCharacterStatViewModel->SetMaxHP(PartyMember->MaxHP);
	PartyCharacterStatViewModel->SetHP(PartyMember->CurrHP);

	// HP 변경 바인딩
	PartyMember->OnHPChanged.AddUObject(this, &AScarletPlayerHUD::OnPartyMemberHPChanged);

	// 위젯 표시
	PartyCharacterStatWidget->SetVisibility(ESlateVisibility::Visible);
}

void AScarletPlayerHUD::HidePartyWidget(APartyMemberBase* PartyMember)
{
	// 델리게이트 해제
	if (CurrentPartyMember)
	{
		CurrentPartyMember->OnHPChanged.RemoveAll(this);
		CurrentPartyMember = nullptr;
	}

	if (PartyCharacterStatWidget)
	{
		PartyCharacterStatWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AScarletPlayerHUD::OnPartyMemberHPChanged(int32 CurrHP, int32 MaxHP)
{
	if (!PartyCharacterStatViewModel)
	{
		return;
	}
	PartyCharacterStatViewModel->SetMaxHP(MaxHP);
	PartyCharacterStatViewModel->SetHP(CurrHP);
}




