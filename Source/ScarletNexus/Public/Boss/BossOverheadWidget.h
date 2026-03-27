// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
 
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BossUI")
	TObjectPtr<AActor> OwnerBoss = nullptr;
};

