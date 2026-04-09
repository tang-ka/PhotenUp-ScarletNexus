// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/FloatingPyramid.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AFloatingPyramid::AFloatingPyramid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SceneComponent를 루트로 → ProceduralMesh를 자식으로 붙임
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	// 루트에 ProceduralMeshComponent 배치
	PyramidMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PyramidMesh"));
 
	// 기본 충돌 (필요 시 PKInteractable과 연동)
	PyramidMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PyramidMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PyramidMesh->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AFloatingPyramid::BeginPlay()
{
	Super::BeginPlay();
	
	BuildPyramidMesh();
	OriginLocation = GetActorLocation();
 
	// 시작 위상을 랜덤으로 → 여러 개 배치해도 동기화되지 않음
	RunningTime = FMath::FRandRange(0.f, PI * 2.f);
}

// ─────────────────────────────────────────────
// 매 프레임: 상하 부유 + 회전
// ─────────────────────────────────────────────
void AFloatingPyramid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;
 
	// ── 위치: 사인파 부유 ──
	const float BobOffset = FMath::Sin(RunningTime * BobSpeed) * BobAmplitude;
	FVector NewLocation = OriginLocation;
	NewLocation.Z += BobOffset;
	SetActorLocation(NewLocation);
 
	// ── 회전: 꾸준한 Yaw + 사인파 Pitch 틸트 ──
	const float YawDelta = YawRotationSpeed * DeltaTime;
	const float TiltAngle = FMath::Sin(RunningTime * TiltSpeed) * TiltAmplitude;
 
	FRotator CurrentRot = GetActorRotation();
	CurrentRot.Yaw += YawDelta;
	CurrentRot.Pitch = TiltAngle;
	SetActorRotation(CurrentRot);
}

// ─────────────────────────────────────────────
// 사각뿔 메시 생성 (4각 밑면 + 꼭짓점 1개 = 피라미드)
// ─────────────────────────────────────────────
void AFloatingPyramid::BuildPyramidMesh()
{
	const float H = BaseHalfSize;
	const float Ht = PyramidHeight;
 
	// ── 정점 (면별로 분리 → 하드 엣지 노멀) ──
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	
	// 헬퍼: 삼각형 하나 추가
	auto AddTri = [&](const FVector& A, const FVector& B, const FVector& C,
					   const FVector2D& uvA, const FVector2D& uvB, const FVector2D& uvC)
	{
		int32 idx = Vertices.Num();
		Vertices.Add(A);
		Vertices.Add(B);
		Vertices.Add(C);
 
		Triangles.Add(idx);
		Triangles.Add(idx + 1);
		Triangles.Add(idx + 2);
 
		FVector N = FVector::CrossProduct(B - A, C - A).GetSafeNormal();
		Normals.Add(N);
		Normals.Add(N);
		Normals.Add(N);
 
		UV0.Add(uvA);
		UV0.Add(uvB);
		UV0.Add(uvC);
	};
 
	// 밑면 꼭짓점
	const FVector V0(-H, -H, 0.f);
	const FVector V1(-H,  H, 0.f);
	const FVector V2( H,  H, 0.f);
	const FVector V3( H, -H, 0.f);
	const FVector Apex(0.f, 0.f, Ht); // 꼭대기
 
	// ── 옆면 4개 ──
	// Front  (V0 → V3 → Apex)
	AddTri(V0, V3, Apex, FVector2D(0, 0), FVector2D(1, 0), FVector2D(0.5f, 1));
	// Right  (V3 → V2 → Apex)
	AddTri(V3, V2, Apex, FVector2D(0, 0), FVector2D(1, 0), FVector2D(0.5f, 1));
	// Back   (V2 → V1 → Apex)
	AddTri(V2, V1, Apex, FVector2D(0, 0), FVector2D(1, 0), FVector2D(0.5f, 1));
	// Left   (V1 → V0 → Apex)
	AddTri(V1, V0, Apex, FVector2D(0, 0), FVector2D(1, 0), FVector2D(0.5f, 1));
 
	// ── 밑면 (삼각형 2개) ──
	// 밑면 노멀은 아래쪽 (−Z)
	AddTri(V0, V1, V2, FVector2D(0, 0), FVector2D(0, 1), FVector2D(1, 1));
	AddTri(V0, V2, V3, FVector2D(0, 0), FVector2D(1, 1), FVector2D(1, 0));
 
	// 메시 섹션 생성
	PyramidMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0,
								   TArray<FColor>(), TArray<FProcMeshTangent>(), true);
 
	// 머티리얼 적용
	if (PyramidMaterial)
	{
		PyramidMesh->SetMaterial(0, PyramidMaterial);
	}
	else
	{
		UMaterialInterface* DefaultMat = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (DefaultMat)
		{
			PyramidMesh->SetMaterial(0, DefaultMat);
		}
	}
}
