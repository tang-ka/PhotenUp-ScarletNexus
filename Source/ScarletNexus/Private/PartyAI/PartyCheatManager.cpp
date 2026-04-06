// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyCheatManager.h"

#include "ScarletNexus.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "PartyAI/PartyMemberBase.h"

void UPartyCheatManager::WatchParty(int32 PartyIndex)
{
	APlayerController* pc = GetOuterAPlayerController();
	if (!pc) return;

	// 레벨 내 PartyMemberBase 전부 수집 : 사실상 하나만 쓸 듯...
	TArray<AActor*> partyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APartyMemberBase::StaticClass(), partyActors);

	if (!partyActors.IsValidIndex(PartyIndex))
	{
		PRINTLOG_GT(TEXT("PartyIndex: %d 없음. 총 %d명"), PartyIndex, partyActors.Num());
		return;
	}

	APartyMemberBase* target = Cast<APartyMemberBase>(partyActors[PartyIndex]);
	if (!target) return;

	// 카메라 없으면 스폰
	if (!SpectatorCam)
	{
		SpectatorCam = GetWorld()->SpawnActor<ACameraActor>();
	}

	// 타겟 머리 위 뒤쪽에 카메라 붙이기 (3인칭)
	SpectatorCam->AttachToActor(target, FAttachmentTransformRules::KeepRelativeTransform);
	SpectatorCam->SetActorRelativeLocation(FVector(-300.f, 0.f, 150.f));
	SpectatorCam->SetActorRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	pc->SetViewTargetWithBlend(SpectatorCam, 0.5f);
	PRINTLOG_GT(TEXT("파티 캐릭터: %s 관전 시작"), *target->GetName());
}

void UPartyCheatManager::WatchSelf()
{
	APlayerController* pc = GetOuterAPlayerController();
	if (!pc) return;

	// 플레이어 뷰로 복귀
	pc->SetViewTargetWithBlend(pc->GetPawn(), 0.5f);

	if (SpectatorCam)
	{
		SpectatorCam->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	}
}
