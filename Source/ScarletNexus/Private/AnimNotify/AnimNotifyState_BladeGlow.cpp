// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_BladeGlow.h"

#include "Player/Component/BladeHandlerComponent.h"

void UAnimNotifyState_BladeGlow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (UBladeHandlerComponent* BladeHandler = Owner->FindComponentByClass<UBladeHandlerComponent>())
	{
		BladeHandler->OnBladeGlowBegin();
	}
}

void UAnimNotifyState_BladeGlow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	if (UBladeHandlerComponent* BladeHandler = Owner->FindComponentByClass<UBladeHandlerComponent>())
	{
		BladeHandler->OnBladeGlowEnd();
	}
}

