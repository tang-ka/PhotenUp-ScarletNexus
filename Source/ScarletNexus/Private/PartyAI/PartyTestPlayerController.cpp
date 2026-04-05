// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyTestPlayerController.h"

#include "PartyAI/PartyCheatManager.h"

APartyTestPlayerController::APartyTestPlayerController()
{
	CheatClass = UPartyCheatManager::StaticClass();
}
