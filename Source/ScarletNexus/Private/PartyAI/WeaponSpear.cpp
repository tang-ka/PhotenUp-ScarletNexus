// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/WeaponSpear.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWeaponSpear::AWeaponSpear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 루트 : 창 메쉬
	SpearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpearMesh"));
	SetRootComponent(SpearMesh);
	SpearMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 창 끝 콜리전 : 소켓 "SpearTip에 붙힘"
	TipCollision = CreateDefaultSubobject<USphereComponent>(TEXT("TipCollision"));
	TipCollision->SetupAttachment(SpearMesh, FName(TEXT("SpearTip")));
	TipCollision->SetSphereRadius(20.f);
	TipCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TipCollision->SetCollisionProfileName(TEXT("PlayerWeapon"));

	// 불꽃 Niagara FX : 동일 소켓에 붙힘
	FireFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFX"));
	FireFX->SetupAttachment(SpearMesh, FName(TEXT("SpearTip")));
	FireFX->SetAutoActivate(false);

	SpearMesh->SetSimulatePhysics(false);
	SpearMesh->SetEnableGravity(false);
}

// Called when the game starts or when spawned
void AWeaponSpear::BeginPlay()
{
	Super::BeginPlay();

	TipCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponSpear::OnTipOverlapBegin);
}

// Called every frame
void AWeaponSpear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSpear::EnableAttackCollision()
{
	HitActors.Empty(); // 히트 리스트 초기화
	TipCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeaponSpear::DisableAttackCollision()
{
	TipCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActors.Empty();
}

void AWeaponSpear::EnableFireFX()
{
	if (FireFX)
	{
		if (FireFXAsset) FireFX->SetAsset(FireFXAsset);
		FireFX->Activate(true);
	}
}

void AWeaponSpear::DisableFireFX()
{
	if (FireFX) FireFX->Deactivate();
}

void AWeaponSpear::OnTipOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor == GetOwner()) return;
	
	if(HitActors.Contains(OtherActor)) return;
	HitActors.Add(OtherActor);

	OnSpearHit.Broadcast(OtherActor);
}
