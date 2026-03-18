// Fill out your copyright notice in the Description page of Project Settings.


#include "PKObject.h"

#include "Components/BoxComponent.h"

// Sets default values
APKObject::APKObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetCollisionProfileName(TEXT("PKObject"));
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(BoxComp);
}

// Called when the game starts or when spawned
void APKObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APKObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APKObject::OnPKPickUped()
{
}

void APKObject::OnPKReleased()
{
}

void APKObject::OnPKThrown(const FVector& ThrowDir, float ThrowForce)
{
}
