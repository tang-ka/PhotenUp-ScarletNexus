// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SNPlayerController.generated.h"

class UDamageAmountWidgetPoolComponent;
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

private:
	// BladeHandlerComponent::OnBladeDamageDealt 수신 핸들러
	void HandleBladeDamageDealt(const FDamageWidgetData& DamageData);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cheat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDamageAmountWidgetPoolComponent> DamageWidgetPoolComp;
	
};
