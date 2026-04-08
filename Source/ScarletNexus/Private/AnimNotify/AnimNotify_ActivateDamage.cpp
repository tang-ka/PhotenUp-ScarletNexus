// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_ActivateDamage.h"

#include "Enemy/EnemyAttackCollision.h"

void UAnimNotify_ActivateDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	AActor* owner = MeshComp->GetOwner();
	if (!owner) return;
	
	if (UEnemyAttackCollision* atkComp = owner->FindComponentByClass<UEnemyAttackCollision>())
	{
		atkComp->ActivateCollision();
	}
}
