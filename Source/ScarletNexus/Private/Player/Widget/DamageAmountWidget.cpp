// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget/DamageAmountWidget.h"

#include "Components/TextBlock.h"

void UDamageAmountWidget::SetDamageData(const int32 InDamageAmount)
{
	txt_DamageAmount->SetText(FText::AsNumber(InDamageAmount));
	txt_DamageAmount->SetRenderTranslation(FVector2D::ZeroVector);
	PlayAnimation(FloatAndFadeAnim);
}

void UDamageAmountWidget::ActivateTracking(const FVector& InWorldLocation)
{
	TrackedWorldLocation = InWorldLocation;
	bIsTracking = true;

	// 초기 위치 즉시 반영 (첫 프레임 깜빡임 방지)
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FVector2D ScreenPos;
		if (PC->ProjectWorldLocationToScreen(TrackedWorldLocation, ScreenPos, true))
		{
			ScreenPos.Y -= 50.f;
			SetPositionInViewport(ScreenPos, true);
		}
	}
}

void UDamageAmountWidget::DeactivateTracking()
{
	bIsTracking = false;
}

void UDamageAmountWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDamageAmountWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsTracking) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	FVector2D ScreenPos;
	if (PC->ProjectWorldLocationToScreen(TrackedWorldLocation, ScreenPos, true))
	{
		ScreenPos.Y -= 50.f;
		SetPositionInViewport(ScreenPos, true);
	}
}

void UDamageAmountWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	
	if (Animation == FloatAndFadeAnim)
	{
		if (OnReturnToPool)
		{
			OnReturnToPool(this);
		}
	}
}
