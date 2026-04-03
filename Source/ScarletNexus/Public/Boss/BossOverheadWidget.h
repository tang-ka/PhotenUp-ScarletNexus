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
	
	// 이름 설정
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	void SetTextName(const FText& Name);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BossUI")
	TObjectPtr<AActor> OwnerBoss = nullptr;
};

