// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyHanabi.h"

#include "PK/PKComponent.h"

APartyHanabi::APartyHanabi()
{
	PKComp = CreateDefaultSubobject<UPKComponent>(TEXT("PKComp"));
}
