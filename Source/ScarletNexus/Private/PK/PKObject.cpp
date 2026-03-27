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
	// BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetCollisionProfileName(TEXT("PKObject"));
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(BoxComp);
}

// Called when the game starts or when spawned
void APKObject::BeginPlay()
{
	Super::BeginPlay();
	
	PRINTLOG_SH(TEXT("PKObject Spawned: %s"), *GetName());
	if (BoxComp)
	{
		BoxComp->OnComponentHit.AddDynamic(this, &APKObject::OnBoxHit);
	}
	
	ObjectState = EPKObjectState::CanBePickedUp;
}

// Called every frame
void APKObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APKObject::CanBePickeduped_Implementation() const
{
	// 상태를 문자로 출력하고 싶다.
	UE_LOG(LogTemp, Warning, TEXT("Current Object State: %s"), *UEnum::GetValueAsString(ObjectState));
	
	return ObjectState == EPKObjectState::CanBePickedUp;
}

void APKObject::OnPKPickuped_Implementation()
{
	if (BoxComp)
	{
		ObjectState = EPKObjectState::IsHeld;
		
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
		ObjectState = EPKObjectState::CoolDown;
		
		// 물리, 중력 on
		BoxComp->SetSimulatePhysics(true);
		BoxComp->SetEnableGravity(true);

		// 홀드 풀리면 다시 충돌 켜기
		// BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComp->SetCollisionProfileName(TEXT("PKObject"));
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ObjectState = EPKObjectState::CanBePickedUp;
#if WITH_EDITOR
		PRINTLOG_GT(TEXT("PK오브젝트: %s 해제"), *this->GetName());
#endif
	}
}

void APKObject::OnPKThrown_Implementation(const FVector& ThrowDir, float ThrowForce)
{
	ObjectState = EPKObjectState::IsUsed;
	
	// 충돌 재활성화
	
	// 물리, 충돌 재활성화
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bUsedObject = true;
}

void APKObject::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                         UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                         const FHitResult& Hit)
{
	// CoolDown(릴리즈 후 낙하) 또는 IsUsed(던져진 후) 상태에서
	// 충돌 노말이 위쪽(바닥 또는 지면)을 향할 때 다시 집을 수 있는 상태로 복귀
	if (ObjectState == EPKObjectState::CoolDown || ObjectState == EPKObjectState::IsUsed)
	{
		// Hit.ImpactNormal.Z > 0.5f : 충돌면이 충분히 수평(바닥)에 가까울 때
		if (Hit.ImpactNormal.Z > 0.5f)
		{
			ObjectState = EPKObjectState::CanBePickedUp;
			bUsedObject = false;
		}
	}
}
