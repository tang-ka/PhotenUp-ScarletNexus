// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossGameplayTags.h"
 
namespace BossTags
{
	// 이벤트
	UE_DEFINE_GAMEPLAY_TAG(Event_PhaseTransition,	"Boss.Event.PhaseTransition");
	UE_DEFINE_GAMEPLAY_TAG(Event_Stagger,			"Boss.Event.Stagger");
	UE_DEFINE_GAMEPLAY_TAG(Event_BrainCrush,		"Boss.Event.BrainCrush");
	UE_DEFINE_GAMEPLAY_TAG(Event_Death,				"Boss.Event.Death");
	UE_DEFINE_GAMEPLAY_TAG(Event_HitReaction,		"Boss.Event.HitReaction");
	UE_DEFINE_GAMEPLAY_TAG(Event_CutsceneStart,		"Boss.Event.CutsceneStart");
	UE_DEFINE_GAMEPLAY_TAG(Event_CutsceneEnd,		"Boss.Event.CutsceneEnd");
	UE_DEFINE_GAMEPLAY_TAG(Event_MapColorChange,	"Boss.Event.MapColorChange");
 
	// Phase 1 공격
	UE_DEFINE_GAMEPLAY_TAG(Attack_TeleportKick,		"Boss.Attack.TeleportKick");
	UE_DEFINE_GAMEPLAY_TAG(Attack_CloneRush,		"Boss.Attack.CloneRush");
	UE_DEFINE_GAMEPLAY_TAG(Attack_AerialElectric,	"Boss.Attack.AerialElectric");
	UE_DEFINE_GAMEPLAY_TAG(Attack_IceSpikes,		"Boss.Attack.IceSpikes");
 
	// Phase 2 추가
	UE_DEFINE_GAMEPLAY_TAG(Attack_ElectricOrbs,		"Boss.Attack.ElectricOrbs");
 
	// Phase 2-2 추가
	UE_DEFINE_GAMEPLAY_TAG(Attack_TelekinesisThrow,	"Boss.Attack.TelekinesisThrow");
 
	// 텔레포트
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Behind,			"Boss.Teleport.Behind");
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Above,			"Boss.Teleport.Above");
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Side,			"Boss.Teleport.Side");
 
	// 상태
	UE_DEFINE_GAMEPLAY_TAG(State_SuperArmor,		"Boss.State.SuperArmor");
	UE_DEFINE_GAMEPLAY_TAG(State_Invincible,		"Boss.State.Invincible");
	UE_DEFINE_GAMEPLAY_TAG(State_Staggered,			"Boss.State.Staggered");
	UE_DEFINE_GAMEPLAY_TAG(State_InCutscene,		"Boss.State.InCutscene");
}