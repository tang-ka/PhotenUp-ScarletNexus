// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ScarletPlayerHUD.generated.h"

class UPartyHUDViewModel;
class UPartyCharacterStatWidget;
class UDamageAmountWidget;
class UPlayerStatWidget;
class UPlayerHUDViewModel;
class APartyMemberBase;

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API AScarletPlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	/** 파티 가입 시 PartyCharacterWidget을 Visible 상태로 전환하고 ViewModel 바인딩 */
	UFUNCTION()
	void ShowPartyWidget(APartyMemberBase* PartyMember);

	/** 파티 탈퇴 시 PartyCharacterWidget을 Hidden 상태로 전환하고 바인딩 해제 */
	UFUNCTION()
	void HidePartyWidget(APartyMemberBase* PartyMember);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerStatWidget> PlayerStatWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPartyCharacterStatWidget> PartyCharacterWidgetClass;

private:
	/** 파티 HP 변경 중계 핸들러 */
	void OnPartyMemberHPChanged(int32 CurrHP, int32 MaxHP);

private:
	UPROPERTY(Transient)
	TObjectPtr<UPlayerStatWidget> PlayerStatWidget;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHUDViewModel> PlayerStatViewModel;
	
	UPROPERTY(Transient)
	TObjectPtr<UPartyCharacterStatWidget> PartyCharacterWidget;

	UPROPERTY(Transient)
	TObjectPtr<UPartyHUDViewModel> PartyViewModel;

	/** 현재 파티원 (언바인딩용) */
	UPROPERTY(Transient)
	TObjectPtr<APartyMemberBase> CurrentPartyMember;
};
