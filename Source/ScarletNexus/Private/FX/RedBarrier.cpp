// Fill out your copyright notice in the Description page of Project Settings.


#include "FX/RedBarrier.h"

#include "Components/BoxComponent.h"


// Sets default values
ARedBarrier::ARedBarrier()
{
	// 충돌 볼륨 : 실제로 막는 역할
	BlockingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingVolume"));
	BlockingVolume->SetBoxExtent(FVector(10.f, 200.f, 200.f));
	BlockingVolume->SetCollisionProfileName(TEXT("InvisibleWall"));
	RootComponent = BlockingVolume;
	
	// 비주얼 메쉬 (Plane)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 기본 Plane 메쉬 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(PlaneMesh.Object);
		// 세로로 세우기 + 스케일
		MeshComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		MeshComp->SetRelativeScale3D(FVector(4.f, 4.f, 4.f));
	}
}

// Called when the game starts or when spawned
void ARedBarrier::BeginPlay()
{
	Super::BeginPlay();
	
	if (BarrierMaterial)
	{
		DynMatInst = UMaterialInstanceDynamic::Create(BarrierMaterial, this);
		DynMatInst->SetVectorParameterValue(TEXT("BarrierColor"), BarrierColor * EmissiveIntensity);
		MeshComp->SetMaterial(0, DynMatInst);
	}
}

// Called every frame
void ARedBarrier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARedBarrier::ActivateBarrier(bool bActivate)
{
	MeshComp->SetVisibility(bActivate);
	BlockingVolume->SetCollisionEnabled(bActivate ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	
	// 페이드 효과를 원하면 타임라인으로 Opacity 파라미터 보간
	if (DynMatInst)
	{
		DynMatInst->SetScalarParameterValue(TEXT("OpacityMultiplier"), bActivate ? 0.5f : 0.f);
	}
}
