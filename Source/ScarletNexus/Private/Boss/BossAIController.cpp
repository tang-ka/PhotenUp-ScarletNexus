// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/BossAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "Boss/BossCharacterBase.h"
#include "Boss/IBossCharacterInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
 
ABossAIController::ABossAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// -------------------------------------------------------
	// StateTree AI Component 생성
	// 이 컴포넌트가 StateTree 에셋을 실행하고
	// AI Controller를 Context로 전달합니다.
	// -------------------------------------------------------
	StateTreeAIComp = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComp"));
 
	// -------------------------------------------------------
	// AI Perception 설정 (선택사항)
	// 카렌 트래버스는 시야 기반으로 플레이어를 추적
	// -------------------------------------------------------
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
 
	// StateTree AI Component가 Possess 시점에 자동으로 StateTree를 시작합니다.
	// StateTree 에셋은 에디터에서 StateTreeAIComp의 Detail 패널에서 지정하세요.
 
	// 보스 캐릭터 초기화 확인
	if (ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(InPawn))
	{
		if (BossConfig)
		{
			IBossCharacterInterface::Execute_InitializeWithConfig(BossChar, BossConfig);
		}
	}
}
 
void ABossAIController::OnUnPossess()
{
	UE_LOG(LogTemp, Log, TEXT("[BossAI] UnPossessed"));
 
	// StateTree AI Component가 자동으로 StateTree를 정지합니다.
	Super::OnUnPossess();
}
 
void ABossAIController::ForcePhaseTransition(EBossPhase NewPhase)
{
	UE_LOG(LogTemp, Warning, TEXT("[BossAI] 디버그: 페이즈 강제 전환 -> %d"),
		static_cast<int32>(NewPhase));
 
	CurrentPhase = NewPhase;
 
	// TODO: StateTree에 페이즈 전환 이벤트를 보내서
	// 전환 연출 State로 진입시키기
	// SendStateTreeEvent(FGameplayTag::RequestGameplayTag("Boss.Event.PhaseTransition"));
}
 
void ABossAIController::SendStateTreeEvent(FGameplayTag EventTag)
{
	if (!StateTreeAIComp)
	{
		return;
	}
 
	// UE 5.7 StateTree는 SendStateTreeEvent를 통해
	// 외부에서 StateTree에 이벤트를 전달할 수 있습니다.
	// 이를 통해 피격, 경직, 브레인 크래시 등의 외부 트리거를
	// StateTree의 Transition 조건으로 사용할 수 있습니다.
 
	// TODO: UE 5.7의 정확한 이벤트 전송 API 확인 후 구현
	// StateTreeAIComp->SendStateTreeEvent(EventTag);
 
	UE_LOG(LogTemp, Log, TEXT("[BossAI] StateTree 이벤트 전송: %s"), *EventTag.ToString());
}
