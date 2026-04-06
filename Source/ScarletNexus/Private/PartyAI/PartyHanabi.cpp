// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyHanabi.h"

#include "ScarletNexus.h"
#include "StateTree.h"
#include "Interface/DamageableHelper.h"
#include "PartyAI/PartyAIComponent.h"
#include "PartyAI/WeaponSpear.h"
#include "PK/PKComponent.h"

// 쿨타임 Key Name 상수
static const FName SK_SpearAttack = TEXT("SpearAttack");
static const FName SK_Psychokinesis = TEXT("Psychokinesis");

APartyHanabi::APartyHanabi()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PKComp = CreateDefaultSubobject<UPKComponent>(TEXT("PKComp"));
	
	/*ConstructorHelpers::FObjectFinder<UStateTree>TempST(TEXT("/Script/StateTreeModule.StateTree'/Game/GT/ST/ST_PartyCharacter.ST_PartyCharacter'"));
	if (TempST.Succeeded())
	{
		PartyAIComp->PartyCharacterST = TempST.Object;
	}*/
}

void APartyHanabi::BeginPlay()
{
	Super::BeginPlay();

	if (!WeaponClass)
	{
		PRINTLOG_GT(TEXT("WeaponClass가 없다"));
	}
	
	// 창 스폰 후 소켓에 붙이기
	FActorSpawnParameters params;
	params.Owner = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWeaponSpear* spear = GetWorld()->SpawnActor<AWeaponSpear>(WeaponClass, FTransform::Identity, params);
	if (spear)
	{
		spear->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		WeaponActor = spear;

		// 히트 이벤트 바인딩
		spear->OnSpearHit.AddDynamic(this, &APartyHanabi::OnSpearHit);

		// 불꽃 FX 즉시 활성 (항상 켜두려면)
		spear->EnableFireFX();
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

	if (AWeaponSpear* spear = Cast<AWeaponSpear>(WeaponActor))
		spear->EnableAttackCollision();
	
	// Hit 판정은 AnimNotify_SpearHit에서 처리
	
	// 쿨타임 시작
	SetCooldown(SK_SpearAttack, SpearAttackCooldown);
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

void APartyHanabi::OnSpearHit(AActor* HitActor)
{
	if (!DamageableHelpers::IsDamageable(HitActor)) return;

	FDamageInfo info;
	info.DamageAmount = ATK;
	info.DamageCauser = this;
	//IDamageable::Execute_ReceiveDamage(HitActor, info);
	DamageableHelpers::ApplyDamage(HitActor, info);
}
