// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PartyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API APartyAIController : public AAIController
{
	GENERATED_BODY()
public:
	APartyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	// AIPerception : 시야로 파티원 감지
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	// 감지 이벤트 콜백
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
