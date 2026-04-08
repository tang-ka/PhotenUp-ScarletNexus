// Fill out your copyright notice in the Description page of Project Settings.


#include "SNPlayerController.h"

#include "ScarletNexus.h"
#include "PartyAI/PartyCheatManager.h"
#include "Player/PlayerKasane.h"
#include "Player/Component/BladeHandlerComponent.h"
#include "Player/Component/DamageAmountWidgetPoolComponent.h"
#include "Player/Widget/Data/DamageWidgetData.h"

ASNPlayerController::ASNPlayerController()
{
	CheatClass = UPartyCheatManager::StaticClass();
	DamageWidgetPoolComp = CreateDefaultSubobject<UDamageAmountWidgetPoolComponent>(TEXT("DamageWidgetPoolComp"));
}

void ASNPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// Kasane 전용 — BladeHandlerComponent의 OnBladeDamageDealt에 바인딩
	if (APlayerKasane* Kasane = Cast<APlayerKasane>(InPawn))
	{
		if (UBladeHandlerComponent* BladeHandler = Kasane->GetBladeHandlerComp())
		{
			BladeHandler->OnBladeDamageDealt.AddUObject(this, &ASNPlayerController::HandleBladeDamageDealt);
		}
	}
}

void ASNPlayerController::HandleBladeDamageDealt(const FDamageWidgetData& DamageData)
{
	DamageWidgetPoolComp->ShowDamageNumber(DamageData);
}

