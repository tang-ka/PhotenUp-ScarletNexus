// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerHanabi.h"

#include "PK/PKComponent.h"

APlayerHanabi::APlayerHanabi()
{
	PKComponent = CreateDefaultSubobject<UPKComponent>(TEXT("PKComponent"));
}

void APlayerHanabi::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerHanabi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
