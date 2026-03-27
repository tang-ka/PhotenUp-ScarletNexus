// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUDWidget.generated.h"



UCLASS()
class SCARLETNEXUS_API UBossHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	// 보스 액터 설정
	UFUNCTION(BlueprintCallable, Category = "BossUI")
	void SetBossActor(AActor* InBossActor);
 
	// HP 비율 (0.0 ~ 1.0) — ProgressBar에 바인딩
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	float GetHPPercent() const;
 
	// HP 텍스트
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	FText GetHPText() const;
 
	// 현재 페이즈 이름
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	FText GetPhaseText() const;
 
	// 보스 이름
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	FText GetBossName() const;
 
	// 보스가 있는지
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	bool IsBossValid() const;
 
	// 보스가 죽었는지
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "BossUI")
	bool IsBossDead() const;
 
protected:
	UPROPERTY(BlueprintReadOnly, Category = "BossUI")
	TObjectPtr<AActor> BossActor = nullptr;
 
	// 보스 이름 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossUI")
	FText BossDisplayName = FText::FromString(TEXT("KAREN TRAVERS"));
};
