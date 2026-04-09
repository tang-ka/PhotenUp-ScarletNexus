// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SNPlayerController.generated.h"

class UDamageAmountWidgetPoolComponent;
class UTargetingViewModel;
class UTargetingUIWidget;
struct FDamageWidgetData;

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API ASNPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASNPlayerController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// BladeHandlerComponent::OnBladeDamageDealt 수신 핸들러
	void HandleBladeDamageDealt(const FDamageWidgetData& DamageData);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDamageAmountWidgetPoolComponent> DamageWidgetPoolComp;

	// TargetingUI MVVM ──────────────────────────────────────────

	/** Blueprint에서 지정할 TargetingUI 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "UI|Targeting")
	TSubclassOf<UTargetingUIWidget> TargetingUIWidgetClass;

	/** PerceptionComponent 델리게이트를 구독하는 ViewModel */
	UPROPERTY(Transient)
	TObjectPtr<UTargetingViewModel> TargetingViewModel;

	/** 화면에 추가되는 타겟팅 UI 위젯 인스턴스 */
	UPROPERTY(Transient)
	TObjectPtr<UTargetingUIWidget> TargetingUIWidget;
};
