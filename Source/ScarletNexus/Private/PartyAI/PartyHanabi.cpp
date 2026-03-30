// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyHanabi.h"

#include "StateTree.h"
#include "Interface/DamageableHelper.h"
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
	if (WeaponClass)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = this;

		WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);

		if (WeaponActor)
		{
			WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			WeaponActor->SetActorRelativeLocation(WeaponLocationOffset);
			WeaponActor->SetActorRelativeRotation(WeaponRotationOffset);
		}
	}
}

void APartyHanabi::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

// 기본 공격 : 근거리 창 공격
void APartyHanabi::Attack()
{
	if (!IsSkillReady(SK_SpearAttack)) return;

	// 애니메이션 몽타주 재생
	// TODO A2, A3 랜덤 재생 추가 예정
	float montageDuration = PlayMontage(AttackA1Montage);
	float cooldown = montageDuration > 0.f ? montageDuration : SpearAttackCooldown;
	
	// Hit 판정은 AnimNotify_SpearHit에서 처리
	
	// 쿨타임 시작
	SetCooldown(SK_SpearAttack, cooldown);
}

int APartyHanabi::GetHanabiATK()
{
	int min = HanabiATK - 10;
	min = FMath::Clamp(min, 0, HanabiATK);
	int max = HanabiATK + 10;
	max = FMath::Clamp(max, 0, HanabiATK);
	int randATK = FMath::RandRange(min, max);
	return randATK;
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
