// Fill out your copyright notice in the Description page of Project Settings.


#include "SNPlayerController.h"

#include "PartyAI/PartyCheatManager.h"

ASNPlayerController::ASNPlayerController()
{
	CheatClass = UPartyCheatManager::StaticClass();
}
