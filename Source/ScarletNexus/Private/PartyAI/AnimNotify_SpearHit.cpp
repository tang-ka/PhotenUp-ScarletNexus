// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/AnimNotify_SpearHit.h"

#include "Interface/DamageableHelper.h"
#include "PartyAI/PartyHanabi.h"
#include "Player/PlayerCharacterBase.h"

void UAnimNotify_SpearHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	APartyHanabi* hanabi = MeshComp ? Cast<APartyHanabi>(MeshComp->GetOwner()) : nullptr;
	if (!hanabi) return;

	TArray<FHitResult> hits;
	hanabi->PerformSpearTrace(hits);

	for (const FHitResult& hit : hits)
	{
		AActor* actor = hit.GetActor();
		if (!actor || actor == hanabi || Cast<APlayerCharacterBase>(actor)) continue;

		// 데미지 전달 함수 호출
		hanabi->ApplyDamageToHitTarget(actor, hanabi, hanabi->GetHanabiATK());
	}
}
