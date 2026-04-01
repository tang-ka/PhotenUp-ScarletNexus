// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/BossAttackCollisionComponent.h"
#include "Interface/DamageableHelper.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UBossAttackCollisionComponent::UBossAttackCollisionComponent()
{
	// 기본 Sphere 크기
	InitSphereRadius(30.f);

	// 콜리전 설정
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetGenerateOverlapEvents(true);

	// 시작 시 비활성화
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetHiddenInGame(true);

	PrimaryComponentTick.bCanEverTick = false;
}

void UBossAttackCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 이벤트 바인딩
	OnComponentBeginOverlap.AddDynamic(this, &UBossAttackCollisionComponent::OnAttackOverlapBegin);
}

void UBossAttackCollisionComponent::EnableAttackCollision(float InDamage, float InKnockback)
{
	CurrentDamage = InDamage;
	CurrentKnockback = InKnockback;
	HitActors.Empty();

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

#if ENABLE_DRAW_DEBUG
	SetHiddenInGame(false);
#endif

	UE_LOG(LogTemp, Log, TEXT("[AttackCollision] %s 활성화 - 데미지: %.0f"), *GetName(), CurrentDamage);
}

void UBossAttackCollisionComponent::DisableAttackCollision()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetHiddenInGame(true);
	HitActors.Empty();

	UE_LOG(LogTemp, Log, TEXT("[AttackCollision] %s 비활성화"), *GetName());
}

void UBossAttackCollisionComponent::ResetHitActors()
{
	HitActors.Empty();
}

void UBossAttackCollisionComponent::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// 자신 무시
	AActor* Owner = GetOwner();
	if (OtherActor == Owner) return;

	// 이미 히트한 액터 무시
	if (HitActors.Contains(OtherActor)) return;

	// IDamageable로 데미지 적용
	if (DamageableHelpers::IsDamageable(OtherActor))
	{
		if (DamageableHelpers::ApplyDamage(OtherActor, Owner, static_cast<int>(CurrentDamage)))
		{
			HitActors.Add(OtherActor);
			UE_LOG(LogTemp, Log, TEXT("[AttackCollision] %s → %s에게 %.0f 데미지!"),
				*GetName(), *OtherActor->GetName(), CurrentDamage);

			// 넉백
			if (CurrentKnockback > 0.f)
			{
				if (ACharacter* HitChar = Cast<ACharacter>(OtherActor))
				{
					if (UCharacterMovementComponent* MoveComp = HitChar->GetCharacterMovement())
					{
						FVector KnockDir = (OtherActor->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
						KnockDir.Z = 0.3f;
						KnockDir.Normalize();
						MoveComp->AddImpulse(KnockDir * CurrentKnockback, true);
					}
				}
			}
		}
	}
}