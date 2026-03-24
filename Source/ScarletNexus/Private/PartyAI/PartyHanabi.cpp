// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyHanabi.h"

#include "StateTree.h"
#include "PartyAI/PartyAIComponent.h"
#include "PK/PKComponent.h"

// 쿨타임 Key Name 상수
static const FName SK_SpearAttack = TEXT("SpearAttack");
static const FName SK_Psychokinesis = TEXT("Psychokinesis");

APartyHanabi::APartyHanabi()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PKComp = CreateDefaultSubobject<UPKComponent>(TEXT("PKComp"));
	
	ConstructorHelpers::FObjectFinder<UStateTree>TempST(TEXT("/Script/StateTreeModule.StateTree'/Game/GT/ST/ST_PartyCharacter.ST_PartyCharacter'"));
	if (TempST.Succeeded())
	{
		PartyAIComp->PartyCharacterST = TempST.Object;
	}
}

void APartyHanabi::BeginPlay()
{
	Super::BeginPlay();
	
	// 무기 붙히기
}

void APartyHanabi::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 잡고 있는 PK오브젝트가 있으면 매 프레임 위치 갱신
	if (PKComp->IsHoldingPKObject())
	{
		
	}
}

// 기본 공격 : 근거리 창 공격
void APartyHanabi::Attack()
{
	if (!IsSkillReady(SK_SpearAttack)) return;
	
	TArray<FHitResult> hitResults;
	PerformSpearTrace(hitResults);
	
	bool bHitAny = false;
	for (const FHitResult& hit : hitResults)
	{
		AActor* actor = hit.GetActor();
		if (!actor || actor == this) continue;
		
		// 데미지 적용
		
		bHitAny = true;
	}
	
	// 쿨타임 시작
	SetCooldown(SK_SpearAttack, SpearAttackCooldown);
	// -> 애니메이션 재생
}

void APartyHanabi::SkillPK()
{
}

void APartyHanabi::PerformSpearTrace(TArray<FHitResult> hitResults)
{
	// 창 끝 위치 : 하나비 앞 방향으로 BoxHalfExtent.X 만큼 앞
	FVector start = GetActorLocation();
	FVector forward = GetActorForwardVector();
	FVector end = start + forward * SpearBoxHalfExtent.X * 2.f;
	
	FQuat rot = GetActorQuat();
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	GetWorld()->SweepMultiByChannel(hitResults, start, end, rot, ECC_Pawn, FCollisionShape::MakeBox(SpearBoxHalfExtent), params);
	
#if WITH_EDITOR
	// 디버그 시각화 (에디터에서만)
	DrawDebugBox(
		GetWorld(),
		(start + end) * 0.5f,
		SpearBoxHalfExtent,
		rot,
		hitResults.Num() > 0 ? FColor::Red : FColor::Green,
		false, 0.5f
	);
#endif
}
