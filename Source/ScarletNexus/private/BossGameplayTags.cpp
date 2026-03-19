// Fill out your copyright notice in the Description page of Project Settings.


#include "BossGameplayTags.h"
 
namespace BossTags
{
	// 이벤트
	UE_DEFINE_GAMEPLAY_TAG(Event_PhaseTransition,   "Boss.Event.PhaseTransition");
	UE_DEFINE_GAMEPLAY_TAG(Event_Stagger,           "Boss.Event.Stagger");
	UE_DEFINE_GAMEPLAY_TAG(Event_BrainCrush,        "Boss.Event.BrainCrush");
	UE_DEFINE_GAMEPLAY_TAG(Event_Death,             "Boss.Event.Death");
	UE_DEFINE_GAMEPLAY_TAG(Event_HitReaction,       "Boss.Event.HitReaction");
 
	// Phase 1 공격
	UE_DEFINE_GAMEPLAY_TAG(Attack_Melee_Combo3Hit,       "Boss.Attack.Melee.Combo3Hit");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Melee_DashSlash,       "Boss.Attack.Melee.DashSlash");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Ranged_PsychicLance,   "Boss.Attack.Ranged.PsychicLance");
 
	// Phase 2 공격
	UE_DEFINE_GAMEPLAY_TAG(Attack_Melee_Combo5Hit,       "Boss.Attack.Melee.Combo5Hit");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Melee_TeleportSlash,   "Boss.Attack.Melee.TeleportSlash");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Ranged_PsychicBarrage, "Boss.Attack.Ranged.PsychicBarrage");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Special_TimeSlash,     "Boss.Attack.Special.TimeSlash");
 
	// Phase 3 공격
	UE_DEFINE_GAMEPLAY_TAG(Attack_Melee_Combo7Hit,       "Boss.Attack.Melee.Combo7Hit");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Special_TimeAccel,     "Boss.Attack.Special.TimeAccel");
	UE_DEFINE_GAMEPLAY_TAG(Attack_Ultimate_Annihilation, "Boss.Attack.Ultimate.Annihilation");
 
	// 텔레포트
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Behind,     "Boss.Teleport.Behind");
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Flank,      "Boss.Teleport.Flank");
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Afterimage, "Boss.Teleport.Afterimage");
	UE_DEFINE_GAMEPLAY_TAG(Teleport_Aerial,     "Boss.Teleport.Aerial");
 
	// 상태
	UE_DEFINE_GAMEPLAY_TAG(State_SuperArmor,  "Boss.State.SuperArmor");
	UE_DEFINE_GAMEPLAY_TAG(State_Invincible,  "Boss.State.Invincible");
	UE_DEFINE_GAMEPLAY_TAG(State_Staggered,   "Boss.State.Staggered");
}
