// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossCloneActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
 
ABossCloneActor::ABossCloneActor()
{
	PrimaryActorTick.bCanEverTick = true;
 
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComp->InitCapsuleSize(42.f, 96.f);
	CapsuleComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = CapsuleComp;
 
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(CapsuleComp);
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
 
	// 분신 반투명 효과 (나중에 머티리얼로 교체)
	// MeshComp->SetScalarParameterValueOnMaterials(TEXT("Opacity"), 0.5f);
}
 
void ABossCloneActor::InitRush(const FVector& InDirection, float InSpeed, float InDistance,
	float InDamage, float InDamageRadius, float InKnockback)
{
	RushDirection = InDirection.GetSafeNormal2D();
	RushSpeed = InSpeed;
	RushDistance = InDistance;
	Damage = InDamage;
	DamageRadius = InDamageRadius;
	KnockbackForce = InKnockback;
	StartLocation = GetActorLocation();
	bRushing = false;
	bRushComplete = false;
	bDamageApplied = false;
 
	// 돌진 방향으로 회전
	SetActorRotation(RushDirection.Rotation());
 
	UE_LOG(LogTemp, Log, TEXT("[BossClone] 분신 스폰 완료 - 위치: %s"),
		*GetActorLocation().ToString());
}
 
void ABossCloneActor::StartRush()
{
	bRushing = true;
	StartLocation = GetActorLocation();
	UE_LOG(LogTemp, Log, TEXT("[BossClone] 분신 돌진 시작!"));
}
 
void ABossCloneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
	if (bRushComplete)
	{
		// 돌진 완료 후 소멸 대기
		DestroyTimer += DeltaTime;
		if (DestroyTimer >= DestroyDelay)
		{
			UE_LOG(LogTemp, Log, TEXT("[BossClone] 분신 소멸"));
			Destroy();
		}
		return;
	}
 
	if (!bRushing)
	{
		return;
	}
 
	// 돌진 이동
	const FVector CurrentLoc = GetActorLocation();
	const FVector NewLoc = CurrentLoc + RushDirection * RushSpeed * DeltaTime;
	const float DistanceTraveled = FVector::Dist2D(StartLocation, NewLoc);
 
	if (DistanceTraveled >= RushDistance)
	{
		// 돌진 완료
		SetActorLocation(StartLocation + RushDirection * RushDistance);
		bRushing = false;
		bRushComplete = true;
		UE_LOG(LogTemp, Log, TEXT("[BossClone] 분신 돌진 완료"));
	}
	else
	{
		SetActorLocation(NewLoc);
 
		// 돌진 중 데미지 판정
		if (!bDamageApplied)
		{
			ApplyRushDamage();
		}
	}
}
 
void ABossCloneActor::ApplyRushDamage()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
 
	// 보스 본체도 무시 (Owner로 설정된 경우)
	if (GetOwner())
	{
		QueryParams.AddIgnoredActor(GetOwner());
	}
 
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DamageRadius),
		QueryParams
	);
 
	for (const FOverlapResult& Overlap : Overlaps)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(Overlap.GetActor());
		if (!HitCharacter)
		{
			continue;
		}
 
		// 보스 본체 제외
		if (GetOwner() && HitCharacter == GetOwner())
		{
			continue;
		}
 
		FDamageEvent DamageEvent;
		HitCharacter->TakeDamage(Damage, DamageEvent, nullptr, this);
 
		UE_LOG(LogTemp, Log, TEXT("[BossClone] %s에게 %.0f 데미지!"),
			*HitCharacter->GetName(), Damage);
 
		if (UCharacterMovementComponent* Movement = HitCharacter->GetCharacterMovement())
		{
			const FVector KBDir = (RushDirection + FVector(0.f, 0.f, 0.3f)).GetSafeNormal();
			Movement->AddImpulse(KBDir * KnockbackForce, true);
		}
 
		bDamageApplied = true;
	}
}
