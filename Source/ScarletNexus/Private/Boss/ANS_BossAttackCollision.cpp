// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/ANS_BossAttackCollision.h"
#include "Boss/BossAttackCollisionComponent.h"


void UANS_BossAttackCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// 이름으로 콜리전 컴포넌트 찾음
	TArray<UBossAttackCollisionComponent*> CollisionComps;
	Owner->GetComponents<UBossAttackCollisionComponent>(CollisionComps);
	

	for (UBossAttackCollisionComponent* Comp : CollisionComps)
	{
		
		if (Comp && Comp->GetName() == CollisionComponentName.ToString())
		{
			Comp->EnableAttackCollision(Damage, Knockback);
			
			return;
		}
	}
}

void UANS_BossAttackCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	TArray<UBossAttackCollisionComponent*> CollisionComps;
	Owner->GetComponents<UBossAttackCollisionComponent>(CollisionComps);

	for (UBossAttackCollisionComponent* Comp : CollisionComps)
	{
		if (Comp && Comp->GetName() == CollisionComponentName.ToString())
		{
			Comp->DisableAttackCollision();
			return;
		}
	}
}

FString UANS_BossAttackCollision::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("AttackCollision: %s"), *CollisionComponentName.ToString());
}