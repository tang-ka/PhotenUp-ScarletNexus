// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyMemberBase.h"

#include "PartyAI/PartyAIComponent.h"

// Sets default values
APartyMemberBase::APartyMemberBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PartyAIComp = CreateDefaultSubobject<UPartyAIComponent>(TEXT("PartyAIComp"));
}

// Called when the game starts or when spawned
void APartyMemberBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APartyMemberBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APartyMemberBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

