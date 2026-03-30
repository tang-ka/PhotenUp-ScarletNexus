// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/BossAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "Boss/BossCharacterBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
 

ABossAIController::ABossAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
	// StateTree AI Component 생성 Statetree실행->AI Controller를 Context로 전달
	
	StateTreeAIComp = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComp"));
 
	
	// 보스 시야 기반으로 플레이어 추적
	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);
 
	// 시야 감각 설정
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 3000.f;           // 탐지 범위
	SightConfig->LoseSightRadius = 3500.f;       // 탐지 유지 범위
	SightConfig->PeripheralVisionAngleDegrees = 120.f; // 시야각
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->SetMaxAge(5.f);                 // 기억 유지 시간
 
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
}
 
void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
 
	UE_LOG(LogTemp, Log, TEXT("[BossAI] Possessed: %s"), *GetNameSafe(InPawn));
 
	// StateTree AI Component가 Possess 시점에 자동으로 StateTree를 시작
 
	// 보스 캐릭터 초기화 확인
	if (ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(InPawn))
	{
		if (BossConfig)
		{
			BossChar->InitializeWithConfig(BossConfig);
		}
	}
}
 
void ABossAIController::OnUnPossess()
{
	UE_LOG(LogTemp, Log, TEXT("[BossAI] UnPossessed"));
 
	// StateTree AI Component가 자동으로 StateTree를 정지.
	Super::OnUnPossess();
}
 
void ABossAIController::ForcePhaseTransition(EBossPhase NewPhase)
{
	UE_LOG(LogTemp, Warning, TEXT("[BossAI] 디버그: 페이즈 강제 전환 -> %d"),
		static_cast<int32>(NewPhase));
 
	CurrentPhase = NewPhase;
	
}
 
void ABossAIController::SendStateTreeEvent(FGameplayTag EventTag)
{
	if (!StateTreeAIComp)
	{
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("[BossAI] StateTree 이벤트 전송: %s"), *EventTag.ToString());
	
	StateTreeAIComp->SendStateTreeEvent(FStateTreeEvent(EventTag));
}