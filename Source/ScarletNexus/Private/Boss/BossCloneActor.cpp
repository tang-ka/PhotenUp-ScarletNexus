// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossCloneActor.h"
#include "Interface/DamageableHelper.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
}
 

void ABossCloneActor::InitRush(const FVector& InDirection, float InSpeed, float InDistance,
	float InDamage, float InDamageRadius, float InKnockback,
	UAnimMontage* InRushMontage)
{
	RushDirection = InDirection.GetSafeNormal2D();
	RushSpeed = InSpeed;
	RushDistance = InDistance;
	Damage = InDamage;
	DamageRadius = InDamageRadius;
	KnockbackForce = InKnockback;
	RushMontage = InRushMontage;
	StartLocation = GetActorLocation();
	bRushing = false;
	bRushComplete = false;
	bDamageApplied = false;

	SetActorRotation(RushDirection.Rotation());
	
	if (RushMontage && MeshComp && MeshComp->GetAnimInstance())
	{
		MeshComp->GetAnimInstance()->Montage_Play(RushMontage);
		MeshComp->GetAnimInstance()->Montage_JumpToSection(FName("WindUp"), RushMontage);
	}

	UE_LOG(LogTemp, Log, TEXT("[BossClone] 분신 스폰 완료 - 위치: %s"),
		*GetActorLocation().ToString());
}
 
void ABossCloneActor::StartRush()
{
	bRushing = true;
	StartLocation = GetActorLocation();

	// 돌진 몽타주 재생
	if (RushMontage && MeshComp && MeshComp->GetAnimInstance())
	{
		MeshComp->GetAnimInstance()->Montage_Play(RushMontage);
		MeshComp->GetAnimInstance()->Montage_JumpToSection(FName("Rush"), RushMontage);
	}

	UE_LOG(LogTemp, Log, TEXT("[BossClone] 분신 돌진 시작!"));
}
 
void ABossCloneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
	if (bRushComplete)
	{
		// 돌진 완료 후 대기
		return;
	}
 
	if (!bRushing)
	{
		return;
	}

	// 돌진 이동
	const FVector CurrentLoc = GetActorLocation();

	// 플레이어 방향으로 약간 보정
	if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (const APawn* Player = PC->GetPawn())
		{
			FVector DesiredDir = (Player->GetActorLocation() - CurrentLoc).GetSafeNormal2D();
			RushDirection = FMath::VInterpNormalRotationTo(
				RushDirection, DesiredDir, DeltaTime, 30.f);
			SetActorRotation(RushDirection.Rotation());
		}
	}

	const FVector NewLoc = CurrentLoc + RushDirection * RushSpeed * DeltaTime;
	const float DistanceTraveled = FVector::Dist2D(StartLocation, NewLoc);
 
	if (DistanceTraveled >= RushDistance)
	{
		// 돌진 완
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
 
	// 보스 본체도 무시
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
 
		// IDamageable 인터페이스로 데미지 적용
		if (DamageableHelpers::ApplyDamage(HitCharacter, GetOwner() ? GetOwner() : this, static_cast<int>(Damage)))
		{
			UE_LOG(LogTemp, Log, TEXT("[BossClone] %s에게 %.0f 데미지!"),
				*HitCharacter->GetName(), Damage);
		}
 
		if (UCharacterMovementComponent* Movement = HitCharacter->GetCharacterMovement())
		{
			const FVector KBDir = (RushDirection + FVector(0.f, 0.f, 0.3f)).GetSafeNormal();
			Movement->AddImpulse(KBDir * KnockbackForce, true);
		}
 
		bDamageApplied = true;
	}
}
