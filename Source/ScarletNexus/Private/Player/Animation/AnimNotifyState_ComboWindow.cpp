// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animation/AnimNotifyState_ComboWindow.h"

#include "ScarletNexus.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Component/ActionManagerComponent.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		PRINTLOG_SH(TEXT("[ComboWindow] NotifyBegin: MeshComp 또는 Owner가 유효하지 않음"));
		return;
	}
	
	auto* Player = Cast<APlayerCharacterBase>(MeshComp->GetOwner());
	if (Player)
	{
		PRINTLOG_SH(TEXT("[ComboWindow(AnimNotify)] 콤보 윈도우 열림"));
		Player->GetActionManagerComp()->OpenComboWindow();
		Player->TryConsumeBufferedAttack();
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner())
	{
		PRINTLOG_SH(TEXT("[ComboWindow] NotifyBegin: MeshComp 또는 Owner가 유효하지 않음"));
		return;
	}
	
	auto* Player = Cast<APlayerCharacterBase>(MeshComp->GetOwner());
	if (Player)
	{
		PRINTLOG_SH(TEXT("[ComboWindow(AnimNotify)] 콤보 윈도우 닫힘"));
		Player->GetActionManagerComp()->CloseComboWindow();
	}
}
