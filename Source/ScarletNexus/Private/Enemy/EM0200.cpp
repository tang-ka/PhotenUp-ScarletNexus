// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EM0200.h"

#include "ScarletNexus.h"
#include "Boss/BossOverheadWidget.h"
#include "Components/WidgetComponent.h"

void AEM0200::BeginPlay()
{
	Super::BeginPlay();
	
	UBossOverheadWidget* ohWidget = Cast<UBossOverheadWidget>(HealthBarComp->GetUserWidgetObject());
	if (ohWidget)
	{
		ohWidget->SetTextName(FText::FromString(this->GetName()));
	}
	else PRINTLOG_GT(TEXT("OverheadWidget이 Null이다. HealthBarComp.GetClass() : %s"), *HPBarWidgetClass->GetName())
}
