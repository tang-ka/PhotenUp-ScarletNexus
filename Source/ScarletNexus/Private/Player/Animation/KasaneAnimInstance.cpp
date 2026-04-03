// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animation/KasaneAnimInstance.h"

void UKasaneAnimInstance::AnimNotify_JumpStart()
{
	bIsJumpEnd = false;
}

void UKasaneAnimInstance::AnimNotify_JumpEnd()
{
	bIsJumpEnd = true;
}