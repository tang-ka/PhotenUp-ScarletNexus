// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetingUIWidget.generated.h"

class UOverlay;
class UWidget;
class UTargetingViewModel;

/**
 * TargetingViewModel의 델리게이트만 구독한다.
 * PlayerPerceptionComponent 및 PlayerCharacterBase를 직접 참조하지 않는다.
 */
UCLASS()
class SCARLETNEXUS_API UTargetingUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** ViewModel 주입 및 델리게이트 바인딩 (ASNPlayerController에서 호출) */
	void SetViewModel(UTargetingViewModel* InViewModel);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// LockOn 활성화 시 HardTarget 위에 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> wbp_HardTargetingUI;

	// PsychokinesisTarget 위에 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> icon_RightClick;

private:
	/** ViewModel → HardTarget 변경 콜백 : 가시성만 제어 */
	void OnHardTargetUpdated(AActor* NewTarget);

	/** ViewModel → PKTarget 변경 콜백 : 가시성만 제어 */
	void OnPKTargetUpdated(AActor* NewTarget);

	/** 월드 좌표 → 스크린 좌표 변환 후 CanvasPanelSlot 위치 갱신 */
	void UpdateWidgetScreenPosition(UWidget* Widget, AActor* Target);

private:
	UPROPERTY()
	TObjectPtr<UTargetingViewModel> ViewModel;

	// NativeDestruct에서 구독 해제를 위한 핸들 보관
	FDelegateHandle HardTargetWidgetHandle;
	FDelegateHandle PKTargetWidgetHandle;

	// NativeTick에서 좌표 계산 스킵 여부 판단용 캐시
	bool bShowHardTarget{false};
	bool bShowPKTarget{false};
};
