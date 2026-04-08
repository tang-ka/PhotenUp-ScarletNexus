// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Widget/WidgetView.h"
#include "Boss/BossTypes.h"
#include "BossHUDWidget.generated.h"

class UBossHUDViewModel;
class UTextBlock;
class UProgressBar;

/**
 * 보스 HUD 위젯 — UWidgetView 기반 MVVM 패턴
 *
 * 데이터 흐름:
 *   UBossHUDViewModel::OnHPUpdated    --> UpdateHP()
 *   UBossHUDViewModel::OnPhaseUpdated --> UpdatePhase()
 */
UCLASS()
class SCARLETNEXUS_API UBossHUDWidget : public UWidgetView
{
	GENERATED_BODY()

public:
	/** ViewModel 바인딩 — AScarletPlayerHUD::BeginPlay 에서 호출 */
	virtual void InitViewModel(UViewModel* InViewModel) override;

	// -- Blueprint Pure 접근자 (캐시 기반) --

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	float GetHPPercent() const { return CachedHPPercent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	FText GetHPText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	FText GetPhaseText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	FText GetBossName() const { return BossDisplayName; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	bool IsBossAlive() const { return bIsBossAlive; }

protected:
	/** ViewModel::OnHPUpdated 수신 → ProgressBar 갱신 */
	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP, float Percent);

	/** ViewModel::OnPhaseUpdated 수신 → 텍스트 갱신 */
	UFUNCTION()
	void UpdatePhase(EBossPhase OldPhase, EBossPhase NewPhase);

	/** 디자이너가 BP 디폴트에서 설정하는 보스 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossUI")
	FText BossDisplayName = FText::FromString(TEXT("KAREN TRAVERS"));

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> pb_BossHP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> txt_Phase;

private:
	float      CachedHPPercent  = 0.f;
	float      CachedCurrentHP  = 0.f;
	float      CachedMaxHP      = 0.f;
	EBossPhase CachedPhase      = EBossPhase::Phase1;
	bool       bIsBossAlive     = false;
};
