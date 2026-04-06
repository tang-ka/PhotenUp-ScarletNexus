// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* pawn = TryGetPawnOwner();
	OwnerEnemy = Cast<AEnemyBase>(pawn);
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerEnemy) return;

	// ** 이동
	const FVector velocity = OwnerEnemy->GetVelocity();
	Speed = velocity.Size2D();
	bIsMoving = Speed > 3.f;

	// 캐릭터 기준 이동 방향
	if (bIsMoving)
	{
		const FRotator actorRot = OwnerEnemy->GetActorRotation();
		const FRotator velRot = velocity.ToOrientationRotator();
		FRotator delta = (velRot - actorRot).GetNormalized();
		Direction = delta.Yaw;
	}
	else
	{
		Direction = 0.f;
	}

	// 공중에 있는지 체크
	if (UCharacterMovementComponent* moveComp = OwnerEnemy->GetCharacterMovement())
	{
		bIsInAir = moveComp->IsFalling();
	}

	// ** 상태
	bIsDie = OwnerEnemy->bIsDie;
	bIsStunned = OwnerEnemy->bIsStunned;
}

void UEnemyAnimInstance::PlayAttackMontage()
{
	if (AttackMontage && !Montage_IsPlaying(AttackMontage))
	{
		Montage_Play(AttackMontage, 1.f);
	}
}

void UEnemyAnimInstance::PlayHitReactMontage()
{
	if (HitReactMontage)
	{
		// 이미 재생 중이면 처음부터 다시 재생
		Montage_Play(HitReactMontage, 1.f);
	}
}

void UEnemyAnimInstance::PlayDieMontage()
{
	if (DieMontage)
	{
		Montage_Play(DieMontage, 1.f);
	}
}
