// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyAIController.h"

#include "ScarletNexus.h"
#include "PartyAI/PartyAIComponent.h"
#include "PartyAI/PartyMemberBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

APartyAIController::APartyAIController()
{
	// Perception 콤포넌트
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);

	// 시야 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.f;		// 감지 반경
	SightConfig->LoseSightRadius = 1500.f;  // 놓치는 거리
	SightConfig->PeripheralVisionAngleDegrees = 60.f; // 시야각
	SightConfig->SetMaxAge(5.f);

	// 감지
	SightConfig->DetectionByAffiliation.bDetectEnemies = false;		// 적
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;	// 중립
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;	// 아군

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	// 감지 이벤트 콜백 등록
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &APartyAIController::OnTargetPerceptionUpdated);
}

void APartyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APartyAIController::OnPossess(APawn* InPawn)
{  
	Super::OnPossess(InPawn);

	PRINTLOG_GT(TEXT("호출됨. Pawn: %s"), *InPawn->GetName());
	
	// State Tree 시작은 PartyAIComponent에서 처리
	if (APartyMemberBase* PMB = Cast<APartyMemberBase>(InPawn))
	{
		// 합류 사운드 재생
		PMB->PlayJoinPartySound();
		
		if (PMB->PartyAIComp)
		{
			PMB->PartyAIComp->StartAI();
		}
	}
}

void APartyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
}
