// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Widget/TargetingUIWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "GameFramework/PlayerController.h"
#include "Player/Widget/TargetingViewModel.h"

void UTargetingUIWidget::SetViewModel(UTargetingViewModel* InViewModel)
{
	// 기존 바인딩이 있다면 먼저 해제
	if (ViewModel)
	{
		ViewModel->OnHardTargetUpdated.Remove(HardTargetWidgetHandle);
		ViewModel->OnPKTargetUpdated.Remove(PKTargetWidgetHandle);
	}

	ViewModel = InViewModel;

	if (!ViewModel)
	{
		return;
	}

	// 새 ViewModel에 가시성 제어 콜백 바인딩
	HardTargetWidgetHandle = ViewModel->OnHardTargetUpdated.AddUObject(
		this, &UTargetingUIWidget::OnHardTargetUpdated);

	PKTargetWidgetHandle = ViewModel->OnPKTargetUpdated.AddUObject(
		this, &UTargetingUIWidget::OnPKTargetUpdated);
}

void UTargetingUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태: 모두 숨김
	if (wbp_HardTargetingUI)
	{
		wbp_HardTargetingUI->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (icon_RightClick)
	{
		icon_RightClick->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTargetingUIWidget::NativeDestruct()
{
	// 위젯 파괴 시 ViewModel 델리게이트 구독 해제 (메모리 누수 방지)
	if (ViewModel)
	{
		ViewModel->OnHardTargetUpdated.Remove(HardTargetWidgetHandle);
		ViewModel->OnPKTargetUpdated.Remove(PKTargetWidgetHandle);
	}

	Super::NativeDestruct();
}

void UTargetingUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ViewModel)
	{
		return;
	}

	// 가시성 캐시가 true인 경우에만 좌표 변환 수행 (불필요한 연산 억제)
	if (bShowHardTarget)
	{
		if (AActor* HardTarget = ViewModel->GetHardTarget())
		{
			UpdateWidgetScreenPosition(wbp_HardTargetingUI, HardTarget);
		}
	}

	if (bShowPKTarget)
	{
		if (AActor* PKTarget = ViewModel->GetPKTarget())
		{
			UpdateWidgetScreenPosition(icon_RightClick, PKTarget);
		}
	}
}

void UTargetingUIWidget::OnHardTargetUpdated(AActor* NewTarget)
{
	// SoftTarget 상태(락온 X)이거나 HardTarget 없으면 숨김
	bShowHardTarget = IsValid(NewTarget);

	if (wbp_HardTargetingUI)
	{
		wbp_HardTargetingUI->SetVisibility(
			bShowHardTarget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		
		if (bShowHardTarget && FlickerAnim)
		{
			PlayAnimation(FlickerAnim);
		}
	}
}

void UTargetingUIWidget::OnPKTargetUpdated(AActor* NewTarget)
{
	bShowPKTarget = IsValid(NewTarget);

	if (icon_RightClick)
	{
		icon_RightClick->SetVisibility(
			bShowPKTarget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		
		if (bShowPKTarget && FadeInAnim)
		{
			PlayAnimation(FadeInAnim);
		}
	}
}

void UTargetingUIWidget::UpdateWidgetScreenPosition(UWidget* Widget, AActor* Target)
{
	if (!Widget || !Target)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
	if (!CanvasSlot)
	{
		return;
	}

	FVector2D ScreenPos;
	const bool bProjected = PC->ProjectWorldLocationToScreen(
		Target->GetActorLocation(), ScreenPos, true);

	if (!bProjected)
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// DPI 보정: 픽셀 좌표 → 슬레이트 좌표
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
	ScreenPos /= DPIScale;

	// Anchor 기준점 보정:
	// CanvasPanelSlot의 Position은 "Anchor 기준점으로부터의 오프셋"이다.
	// Anchor가 (0,0)이면 보정 없음, (0.5,0.5)이면 캔버스 중앙이 기준이 되어
	// SetPosition(ScreenPos) 만으로는 위치가 어긋난다.
	const FVector2D CanvasSize = GetCachedGeometry().GetLocalSize();
	const FAnchors Anchors = CanvasSlot->GetAnchors();
	const FVector2D AnchorOffset(
		Anchors.Minimum.X * CanvasSize.X,
		Anchors.Minimum.Y * CanvasSize.Y
	);

	// 위젯 중심을 타겟에 정렬 (Alignment 0.5,0.5 기준)
	CanvasSlot->SetPosition(ScreenPos - AnchorOffset);
	Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
}
