// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKObject.h"

#include "ScarletNexus.h"
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
		ObjectState = EPKObjectState::IsHeld;
#if WITH_EDITOR
		PRINTLOG_GT(TEXT("PK오브젝트: %s 픽업"), *this->GetName());
#endif
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
		ObjectState = EPKObjectState::CanBePickedUp;
#if WITH_EDITOR
		PRINTLOG_GT(TEXT("PK오브젝트: %s 해제"), *this->GetName());
#endif
	}
}

void APKObject::OnPKThrown_Implementation(const FVector& ThrowDir, float ThrowForce)
{
	// 물리, 충돌 재활성화
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bUsedObject = true;
	ObjectState = EPKObjectState::IsUsed;
	// TODO 사라지는 처리 
	//Destroy();
#if WITH_EDITOR
	PRINTLOG_GT(TEXT("PK오브젝트: %s 투척 | 방향 : X:%1.f, Y:%1.f, Z:%1.f | 강도 : %1.f"), *this->GetName(), ThrowDir.X, ThrowDir.Y, ThrowDir.Z, ThrowForce);
#endif
}