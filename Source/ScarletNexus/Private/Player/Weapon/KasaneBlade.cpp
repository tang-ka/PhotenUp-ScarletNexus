// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/KasaneBlade.h"

#include "Components/SphereComponent.h"


AKasaneBlade::AKasaneBlade()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	
	BladeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BladeMesh"));
	RootComponent = BladeMesh;
}

void AKasaneBlade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKasaneBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

