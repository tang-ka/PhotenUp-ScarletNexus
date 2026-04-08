// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ScarletPlayerHUD.generated.h"

class UBossHUDWidget;
class UBossHUDViewModel;
class UPartyHUDViewModel;
class UPartyCharacterStatWidget;
class UDamageAmountWidget;
class UPlayerStatWidget;
class UPlayerHUDViewModel;
class APartyMemberBase;
class ABossCharacterBase;

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API AScarletPlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	/** 파티 가입 시 PartyCharacterWidget을 Visible 상태로 전환하고 ViewModel 바인딩 */
	UFUNCTION()
	void ShowPartyWidget(APartyMemberBase* PartyMember);

	/** 파티 탈퇴 시 PartyCharacterWidget을 Hidden 상태로 전환하고 바인딩 해제 */
	UFUNCTION()
	void HidePartyWidget(APartyMemberBase* PartyMember);

	/**
	 * 보스 등록 — 레벨 시작 시 자동 탐색되지만,
	 * 보스가 동적으로 스폰될 경우 외부에서 수동 호출 가능
	 */
	UFUNCTION(BlueprintCallable, Category = "BossUI")
	void RegisterBoss(ABossCharacterBase* BossCharacter);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerStatWidget> PlayerStatWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPartyCharacterStatWidget> PartyCharacterWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBossHUDWidget> BossHUDWidgetClass;

private:
	/** 파티 HP 변경 중계 핸들러 */
	void OnPartyMemberHPChanged(int32 CurrHP, int32 MaxHP);

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerStatWidget> PlayerStatWidget;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHUDViewModel> PlayerStatViewModel;
	
	UPROPERTY(Transient)
	TObjectPtr<UPartyCharacterStatWidget> PartyCharacterStatWidget;

	UPROPERTY(Transient)
	TObjectPtr<UPartyHUDViewModel> PartyCharacterStatViewModel;
	
	UPROPERTY(Transient)
	TObjectPtr<UBossHUDWidget> BossHUDWidget;

	UPROPERTY(Transient)
	TObjectPtr<UBossHUDViewModel> BossHUDViewModel;

	/** 현재 등록된 보스 (언바인딩용) */
	UPROPERTY(Transient)
	TObjectPtr<ABossCharacterBase> CurrentBoss;

	/** 현재 파티원 (언바인딩용) */
	UPROPERTY(Transient)
	TObjectPtr<APartyMemberBase> CurrentPartyMember;
};
