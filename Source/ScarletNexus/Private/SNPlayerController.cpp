// Fill out your copyright notice in the Description page of Project Settings.


#include "SNPlayerController.h"

#include "ScarletNexus.h"
#include "PartyAI/PartyCheatManager.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/PlayerKasane.h"
#include "Player/Component/BladeHandlerComponent.h"
#include "Player/Component/DamageAmountWidgetPoolComponent.h"
#include "Player/Component/PlayerPerceptionComponent.h"
#include "Player/Component/PsychokinesisComponent.h"
#include "Player/Widget/Data/DamageWidgetData.h"
#include "Player/Widget/TargetingUIWidget.h"
#include "Player/Widget/TargetingViewModel.h"

ASNPlayerController::ASNPlayerController()
{
	CheatClass = UPartyCheatManager::StaticClass();
	DamageWidgetPoolComp = CreateDefaultSubobject<UDamageAmountWidgetPoolComponent>(TEXT("DamageWidgetPoolComp"));
}

void ASNPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// ── DamageUI : Kasane 전용 ──────────────────────────────────
	if (APlayerKasane* Kasane = Cast<APlayerKasane>(InPawn))
	{
		if (UBladeHandlerComponent* BladeHandler = Kasane->GetBladeHandlerComp())
		{
			BladeHandler->OnBladeDamageDealt.AddUObject(this, &ASNPlayerController::HandleBladeDamageDealt);
		}
	}

	// ── DamageUI : PK 전용 ──────────────────────────────────────
	if (APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(InPawn))
	{
		if (UPsychokinesisComponent* PKComp = PlayerChar->GetPsychokinesisComp())
		{
			PKComp->OnPKDamageDealt.AddUObject(this, &ASNPlayerController::HandleBladeDamageDealt);
		}
	}

	// ── TargetingUI MVVM : APlayerCharacterBase를 소유하는 경우 ──
	if (APlayerCharacterBase* PlayerChar = Cast<APlayerCharacterBase>(InPawn))
	{
		if (UPlayerPerceptionComponent* PerceptionComp = PlayerChar->GetPerceptionComp())
		{
			// 1. ViewModel 생성 및 PerceptionComponent 델리게이트 구독
			TargetingViewModel = NewObject<UTargetingViewModel>(this);
			TargetingViewModel->Initialize(PerceptionComp);

			// 2. Widget 생성 및 ViewModel 주입 후 뷰포트에 추가
			if (TargetingUIWidgetClass)
			{
				TargetingUIWidget = CreateWidget<UTargetingUIWidget>(this, TargetingUIWidgetClass);
				if (TargetingUIWidget)
				{
					TargetingUIWidget->SetViewModel(TargetingViewModel);
					TargetingUIWidget->AddToViewport();
				}
			}
		}
	}
}

void ASNPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// TargetingViewModel 델리게이트 구독 해제
	if (TargetingViewModel)
	{
		TargetingViewModel->Shutdown();
		TargetingViewModel = nullptr;
	}

	// TargetingUIWidget 뷰포트에서 제거
	if (TargetingUIWidget)
	{
		TargetingUIWidget->RemoveFromParent();
		TargetingUIWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ASNPlayerController::HandleBladeDamageDealt(const FDamageWidgetData& DamageData)
{
	DamageWidgetPoolComp->ShowDamageNumber(DamageData);
}

