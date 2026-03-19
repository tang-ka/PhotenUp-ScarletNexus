// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
#include "NativeGameplayTags.h"
 
namespace BossTags
{
	// 보스 이벤트 태그 (StateTree Transition 트리거)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_PhaseTransition);   // Boss.Event.PhaseTransition
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Stagger);           // Boss.Event.Stagger
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_BrainCrush);        // Boss.Event.BrainCrush
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);             // Boss.Event.Death
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitReaction);       // Boss.Event.HitReaction
	
	// 공격 패턴 태그
 
	// Phase 1 패턴
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Melee_Combo3Hit);       // 3타 기본 콤보
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Melee_DashSlash);       // 대시 베기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Ranged_PsychicLance);   // 초뇌력 창
 
	// Phase 2 패턴 (Phase 1 + 추가)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Melee_Combo5Hit);       // 5타 강화 콤보
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Melee_TeleportSlash);   // 텔레포트 연계 베기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Ranged_PsychicBarrage); // 초뇌력 연사
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Special_TimeSlash);     // 시간 베기 (넓은 범위)
 
	// Phase 3 패턴 (전 페이즈 + 추가)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Melee_Combo7Hit);       // 7타 각성 콤보
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Special_TimeAccel);     // 시간 가속 (연속 텔레포트 공격)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack_Ultimate_Annihilation); // 궁극기: 전멸
	
	// 텔레포트 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Behind);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Flank);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Afterimage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Teleport_Aerial);
 
	
	// 상태 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_SuperArmor);    // 슈퍼아머 활성
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invincible);    // 무적 (페이즈 전환 중)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Staggered);     // 경직 상태
}