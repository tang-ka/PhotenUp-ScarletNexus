// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageAmountWidget.generated.h"

struct FDamageWidgetData;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API UDamageAmountWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetDamageData(int32 InDamageAmount);

	// 위치 추적 활성화 (ShowDamageNumber 호출 시)
	void ActivateTracking(const FVector& InWorldLocation);
	// 위치 추적 비활성화 (ReturnToPool 시)
	void DeactivateTracking();
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	TFunction<void(UDamageAmountWidget*)> OnReturnToPool;

protected:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> txt_DamageAmount;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FloatAndFadeAnim;

	// 추적 대상 월드 위치
	FVector TrackedWorldLocation = FVector::ZeroVector;
	bool bIsTracking = false;
};
