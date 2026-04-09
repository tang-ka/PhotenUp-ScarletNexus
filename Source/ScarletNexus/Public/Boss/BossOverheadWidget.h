// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BossOverheadWidget.generated.h"

UCLASS()
class SCARLETNEXUS_API UBossOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
 
public:
	// 오너 보스 설정
	UFUNCTION(BlueprintCallable, Category = "BossUI")
	void SetOwnerBoss(AActor* InBossActor);
 
	// HP 비율
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	float GetHPPercent() const;
 
	// 이름 텍스트
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextName;
	
	// 프로그래스 바
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* ProgressBarHP;
	
	// 이름 설정
	UFUNCTION(BlueprintCallable, Category = "BossUI")
	void SetTextName(const FText& Name);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "BossUI")
	TObjectPtr<AActor> OwnerBoss = nullptr;
	
	// 현재 HP 비율 (위젯 초기화 시 1.0)
	UPROPERTY(BlueprintReadOnly, Category = "BossUI")
	float CachedHPPercent = 1.f;
};

