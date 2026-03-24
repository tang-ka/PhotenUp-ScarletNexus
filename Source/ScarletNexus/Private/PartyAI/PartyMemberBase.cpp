// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyMemberBase.h"

#include "PartyAI/PartyAIComponent.h"

// Sets default values
APartyMemberBase::APartyMemberBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PartyAIComp = CreateDefaultSubobject<UPartyAIComponent>(TEXT("PartyAIComp"));
}

// Called when the game starts or when spawned
void APartyMemberBase::BeginPlay()
{
	Super::BeginPlay();
	CurrHP = MaxHP;
}

// Called every frame
void APartyMemberBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APartyMemberBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 스탯
void APartyMemberBase::TakeDamage_Party(int Damage)
{
	if (!IsAlive()) return;
	CurrHP = FMath::Clamp(CurrHP - Damage, 0, MaxHP);
	
	// 데미지 감소 후 HP가 0 이하면 사망 처리
	if (!IsAlive())
	{
		
	}
}

// 쿨타임
void APartyMemberBase::SetCooldown(FName SkillName, float Duration)
{
	float now = GetWorld()->GetTimeSeconds();
	CooldownEndTimes.Add(SkillName, now + Duration);
}

bool APartyMemberBase::IsSkillReady(FName SkillName) const
{
	const float* endTime = CooldownEndTimes.Find(SkillName);
	if (!endTime) return true; // 아직 한번도 사용하지 않은 상태
	
	float now = GetWorld()->GetTimeSeconds();
	return now >= *endTime;
}

float APartyMemberBase::GetRemainCooldown(FName SkillName) const
{
	const float* endTime = CooldownEndTimes.Find(SkillName);
	if (!endTime) return 0;
	
	float now = GetWorld()->GetTimeSeconds();
	return FMath::Max(0, *endTime - now);
}
