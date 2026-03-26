// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKObject.h"

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

bool APKObject::CanBePickeduped_Implementation() const
{
	return ObjectState == EPKObjectState::CanBePickedUp;
}

void APKObject::OnPKPickuped_Implementation()
{
	if (BoxComp)
	{
		// 물리, 중력 off
		BoxComp->SetSimulatePhysics(false);
		BoxComp->SetEnableGravity(false);

		// 홀드 중에는 충돌 끄기
		BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APKObject::OnPKReleased_Implementation()
{
	if (BoxComp)
	{
		// 물리, 중력 on
		BoxComp->SetSimulatePhysics(true);
		BoxComp->SetEnableGravity(true);

		// 홀드 풀리면 다시 충돌 켜기
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void APKObject::OnPKThrown_Implementation(const FVector& ThrowDir, float ThrowForce)
{
	// 물리, 중력 on
	// 충돌 재활성화
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// Impulse는 PKComponent::UseHeldTarget에서 적용
	bUsedObject = true;
}