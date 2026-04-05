// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animation/AnimNotifyState_ActivateBladeCollision.h"

#include "ScarletNexus.h"
#include "Player/PlayerKasane.h"
#include "Player/Component/BladeHandlerComponent.h"


void UAnimNotifyState_ActivateBladeCollision::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                                          UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		PRINTLOG_SH(TEXT("[AnimNotifyState_ActivateBladeCollision] MeshComp 또는 소유자 없음"));
		return;
	}
	
	auto* Player = Cast<APlayerKasane>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetBladeHandlerComp()->SetActiveAllBladesCollision(true);
	}
}

void UAnimNotifyState_ActivateBladeCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		PRINTLOG_SH(TEXT("[AnimNotifyState_ActivateBladeCollision] MeshComp 또는 소유자 없음"));
		return;
	}
	
	auto* Player = Cast<APlayerKasane>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetBladeHandlerComp()->SetActiveAllBladesCollision(false);
	}
}
