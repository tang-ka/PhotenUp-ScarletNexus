// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "CoreMinimal.h"
#include "AIController.h"
#include "BossTypes.h"
#include "BossAIController.generated.h"
 
class UStateTreeAIComponent;
class UBossConfigDataAsset;
 
UCLASS()
class SCARLETNEXUS_API ABossAIController : public AAIController
{
	GENERATED_BODY()
 
public:
	ABossAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
 
	
	// Lifecycle
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
 
	
	// 외부에서 호출할 수 있는 인터페이스
 
	// 현재 페이즈 조회
	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	EBossPhase GetCurrentPhase() const { return CurrentPhase; }
 
	// 페이즈 강제 전환 (디버그용)
	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Debug")
	void ForcePhaseTransition(EBossPhase NewPhase);
 
	// StateTree에 이벤트 전송 (피격, 경직 등 외부 트리거)
	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	void SendStateTreeEvent(FGameplayTag EventTag);
 
protected:
	
	// Components
 
	// StateTree AI Component - StateTree 실행의 핵심
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|AI")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComp;
 
	
	// Config
 
	// 보스 설정 Data Asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Config")
	TObjectPtr<UBossConfigDataAsset> BossConfig;
 
	
	// Runtime State
 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|State")
	EBossPhase CurrentPhase = EBossPhase::Phase1;
};