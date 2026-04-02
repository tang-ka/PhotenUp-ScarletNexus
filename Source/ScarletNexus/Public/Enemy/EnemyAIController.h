// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
private:
	// AIPerception : 시야로 플레이어 감지
	UPROPERTY(VisibleAnywhere, Category="AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	
	UPROPERTY(VisibleAnywhere, Category="AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
	
	// State Tree 콤포넌트 (Pawn에 붙이지 않고 Controller에서 관리)
	UPROPERTY(VisibleAnywhere, Category="AI")
	TObjectPtr<class UStateTreeComponent> StateTreeComp;
	
	// 에디터에서 할당할 StateTree 에셋
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<class UStateTree> EnemyStateTree;
	
	// 감지 이벤트 콜백
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
