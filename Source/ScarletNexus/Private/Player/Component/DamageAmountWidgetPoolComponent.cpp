// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/DamageAmountWidgetPoolComponent.h"

#include "ScarletNexus.h"
#include "Blueprint/UserWidget.h"
#include "Player/Widget/DamageAmountWidget.h"
#include "Player/Widget/Data/DamageWidgetData.h"


UDamageAmountWidgetPoolComponent::UDamageAmountWidgetPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // 위젯이 스스로 위치를 갱신하므로 컴포넌트 틱 불필요
}

void UDamageAmountWidgetPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializePool();
}

void UDamageAmountWidgetPoolComponent::ShowDamageNumber(const FDamageWidgetData& InData)
{
	UDamageAmountWidget* Widget = GetFromPool();
	if (!Widget) return;

	Widget->SetVisibility(ESlateVisibility::Visible);
	Widget->ActivateTracking(InData.WorldLocation); // 위젯 스스로 위치 추적 시작
	Widget->SetDamageData(InData.DamageAmount);
}

void UDamageAmountWidgetPoolComponent::InitializePool()
{
	for (int32 i = 0; i < PoolSize; i++)
	{
		CreateNewWidgetToPool();
	}
}

UDamageAmountWidget* UDamageAmountWidgetPoolComponent::GetFromPool()
{
	if (AvailablePool.Num() <= 0)
	{
		CreateNewWidgetToPool(); // 풀에서 사용할 수 있는 위젯이 없으면 새로 생성하여 풀에 추가
	}

	if (AvailablePool.Num() <= 0)
	{
		PRINTLOG_SH(TEXT("[DamageWidgetPool] 위젯 생성에 실패 했습니다."));
		return nullptr;
	}
	
	UDamageAmountWidget* Widget = AvailablePool.Pop();
	ActivePool.Add(Widget);
	return Widget;
}

void UDamageAmountWidgetPoolComponent::ReturnToPool(UDamageAmountWidget* Widget)
{	
	int32 Removed = ActivePool.RemoveSingle(Widget);
	if (Removed > 0)
	{
		Widget->DeactivateTracking(); // 위치 추적 중단
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		AvailablePool.Add(Widget);
	}
}

void UDamageAmountWidgetPoolComponent::CreateNewWidgetToPool()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;
	
	UDamageAmountWidget* Widget = CreateWidget<UDamageAmountWidget>(PC, DamageWidgetClass);
	Widget->AddToViewport();
	Widget->SetVisibility(ESlateVisibility::Collapsed);
	Widget->OnReturnToPool = [this](UDamageAmountWidget* W)
	{
		ReturnToPool(W);
	};
	
	AvailablePool.Add(Widget);
}


