// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "NativeGameplayTags.h"
 
namespace BossTags
{
	
	// 보스 이벤트 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_PhaseTransition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Stagger);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_BrainCrush);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitReaction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CutsceneStart);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CutsceneEnd);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_MapColorChange);
 
	
	// Phase 1 공격 패턴 (HP 100% ~ 70%)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_TeleportKick);		// 순간이동 -> 옆차기 -> 뒤돌려차기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_CloneRush);			// 뒤 텔레포트 -> 3분신 순차 돌진
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_AerialElectric);		// 위 텔레포트 -> 바닥 전류 방출
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_IceSpikes);			// 바닥 얼음가시 생성
 
	
	// Phase 2 추가 패턴 (HP 70% ~ 50%)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_ElectricOrbs);		// 전류구 5개 -> 투척
 
	
	// Phase 2-2 추가 패턴 (HP 50% ~ 25%)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_TelekinesisThrow);	// 사물 염동력 던지기
 
	
	// 텔레포트 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Behind);			// 뒤쪽 (분신 돌진 전)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Above);				// 위쪽 (전류 방출 전)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Side);				// 옆 (옆차기 전)
 
	
	// 상태 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_SuperArmor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invincible);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Staggered);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_InCutscene);
}