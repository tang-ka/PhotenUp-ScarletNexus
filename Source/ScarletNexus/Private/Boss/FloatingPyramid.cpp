// FloatingPyramid.cpp

#include "Boss/FloatingPyramid.h"
#include "Engine/StaticMesh.h"

AFloatingPyramid::AFloatingPyramid()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SceneRoot);

	// 엔진 기본 콘 메시 로드 (4각뿔과 유사한 형태)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(
		TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(ConeMesh.Object);
	}

	// 뒤집어서 꼭짓점이 아래를 향하게 (카렌전 스타일)
	MeshComp->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));
}

void AFloatingPyramid::BeginPlay()
{
	Super::BeginPlay();

	OriginLocation = GetActorLocation();
	RunningTime = FMath::FRandRange(0.f, PI * 2.f);

	UE_LOG(LogTemp, Warning, TEXT("[FloatingPyramid] Spawned at %s"), *OriginLocation.ToString());
}

void AFloatingPyramid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	// ── 상하 부유 ──
	const float BobOffset = FMath::Sin(RunningTime * BobSpeed) * BobAmplitude;
	FVector NewLocation = OriginLocation;
	NewLocation.Z += BobOffset;
	SetActorLocation(NewLocation);

	// ── Yaw 회전 + Pitch 틸트 ──
	FRotator CurrentRot = GetActorRotation();
	CurrentRot.Yaw += YawRotationSpeed * DeltaTime;
	CurrentRot.Pitch = FMath::Sin(RunningTime * TiltSpeed) * TiltAmplitude;
	SetActorRotation(CurrentRot);
}