// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/KasaneBlade.h"

#include "ScarletNexus.h"
#include "Components/SphereComponent.h"
#include "Interface/DamageableHelper.h"


AKasaneBlade::AKasaneBlade()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetCollisionProfileName(TEXT("PlayerWeapon"));
	SphereComp->SetSphereRadius(20.f);

	BladeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BladeMesh"));
	BladeMesh->SetupAttachment(RootComponent);
	BladeMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AKasaneBlade::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AKasaneBlade::OnSphereOverlap);
}

void AKasaneBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurState)
	{
	case EBladeState::Idle:
		TickIdle(DeltaTime);
		break;
	case EBladeState::Attack:
		TickAttack(DeltaTime);
		break;
	case EBladeState::Return:
		TickReturn(DeltaTime);
		break;
	case EBladeState::Inactive:
	default:
		break;
	}

	// =============================================
	// 1. 이동 방향을 바라봄 (접선 방향)
	// =============================================
	if (IsMoving())
	{
		FVector MoveDelta = GetActorLocation() - PrevLocation;
		if (!MoveDelta.IsNearlyZero(0.1f))
		{
			FRotator DesiredRotation = MoveDelta.Rotation();
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, 8.f));
		}
		PrevLocation = GetActorLocation();
	}
}

bool AKasaneBlade::IsMoving() const
{
	return CurState == EBladeState::Idle ||
		CurState == EBladeState::Attack ||
		CurState == EBladeState::Return;
}

void AKasaneBlade::SetActive(bool bActivate)
{
	SetActorHiddenInGame(!bActivate);
	SetActorEnableCollision(bActivate);
	SetActorTickEnabled(bActivate);
}

void AKasaneBlade::ChangeBladeState(EBladeState NewState)
{
	if (CurState == NewState)
	{
		return;
	}

	switch (NewState)
	{
	case EBladeState::Idle:
		BeginIdle();
		break;
	case EBladeState::Attack:
		BeginAttack();
		break;
	case EBladeState::Return:
		BeginReturn();
		break;
	case EBladeState::Inactive:
		SetActive(false);
		break;
	default:
		break;
	}

	CurState = NewState;
}

void AKasaneBlade::Init(AActor* InOwner, int32 Index)
{
	OwnerActor = InOwner;
	BladeIndex = Index;
}

void AKasaneBlade::InitForIdle(int32 InIdleBladeCount)
{
	// 인덱스마다 다른 위상으로 Bobbing 비동기화
	IdleBladeCount = InIdleBladeCount;
	PhaseOffset = (2.f * PI * BladeIndex) / InIdleBladeCount;
	ChangeBladeState(EBladeState::Idle);
}

void AKasaneBlade::LaunchAttack(EBladeAttackPattern InPattern, FVector InOrigin, FVector InDirection,
                                float InMaxDistance, float InSpeed)
{
	AttackOrigin = InOrigin;
	AttackDirection = InDirection.GetSafeNormal();
	AttackMaxDistance = InMaxDistance;
	AttackSpeed = InSpeed;
	AttackElapsed = 0.f;

	CurAttackPattern = InPattern;

	SetActorLocation(InOrigin);
	SetActive(true);
	ChangeBladeState(EBladeState::Attack);
}

void AKasaneBlade::BeginIdle()
{
	SetActive(true);
}

void AKasaneBlade::BeginAttack()
{
}

void AKasaneBlade::BeginReturn()
{
	ReturnElapsed = 0.f;
	P0 = GetActorLocation();

	// 구 위의 랜덤 오프셋을 BeginReturn에서 한 번만 결정
	FVector RandomDir = FMath::VRand();  // 구면 균등 랜덤 방향
	ReturnTargetOffset = RandomDir * ReturnArrivalRadius; 
	
	// // 발사 방향 기준 좌우 판별
	// // AttackDirection과 Owner→블레이드 방향의 외적으로 좌우 결정
	// if (OwnerActor.IsValid())
	// {
	// 	FVector ToOwner = (OwnerActor->GetActorLocation() - P0).GetSafeNormal();
	// 	FVector Cross = FVector::CrossProduct(AttackDirection, ToOwner);
	//
	// 	// Cross.Z > 0 이면 왼쪽, < 0 이면 오른쪽
	// 	ReturnCurveSide = (Cross.Z >= 0.f) ? 1.f : -1.f;
	// }
	// else
	// {
	// 	ReturnCurveSide = (FMath::RandBool()) ? 1.f : -1.f;
	// }
}

void AKasaneBlade::TickIdle(float DeltaTime)
{
	if (!OwnerActor.IsValid())
	{
		return;
	}

	AActor* MyOwner = OwnerActor.Get();
	FVector OwnerLocation = MyOwner->GetActorLocation();
	FRotator OwnerRotation = MyOwner->GetActorRotation();
	float Time = GetWorld()->GetTimeSeconds();

	// =============================================
	// 3. 궤도 중심: Owner 뒤쪽 + 위아래 Sin 부유
	// =============================================
	FVector OrbitCenterLocal = FVector(-50.f, 0.f, OrbitHeightOffset); // 캐릭터 뒤쪽
	OrbitCenterLocal.Z += FMath::Sin(Time * BobSpeed + PhaseOffset) * BobAmplitude;

	FVector OrbitCenter = OwnerLocation + OwnerRotation.RotateVector(OrbitCenterLocal);

	// =============================================
	// 2. 회전축: 기본 Up에서 천천히 기울어짐
	// =============================================
	float TiltX = FMath::Sin(Time * FMath::DegreesToRadians(AxisTiltSpeed)) * AxisTiltAmount;
	float TiltY = FMath::Cos(Time * FMath::DegreesToRadians(AxisTiltSpeed) * 0.7f) * AxisTiltAmount;

	FRotator TiltRotation(TiltX, 0.f, TiltY);
	FVector OrbitAxis = TiltRotation.RotateVector(FVector::UpVector);
	// Owner 회전도 반영
	OrbitAxis = OwnerRotation.RotateVector(OrbitAxis);

	// =============================================
	// 공전 각도 계산
	// =============================================
	float AngleDeg = Time * OrbitSpeed + (360.f * BladeIndex / IdleBladeCount);
	float AngleRad = FMath::DegreesToRadians(AngleDeg);

	// OrbitAxis를 법선으로 하는 평면 위에서 원 궤도 계산
	// OrbitAxis에 수직인 두 벡터(Right, Forward) 구하기
	FVector ArbitraryVec = (FMath::Abs(FVector::DotProduct(OrbitAxis, FVector::ForwardVector)) < 0.99f)
		                       ? FVector::ForwardVector
		                       : FVector::RightVector;

	FVector OrbitRight = FVector::CrossProduct(OrbitAxis, ArbitraryVec).GetSafeNormal();
	FVector OrbitForward = FVector::CrossProduct(OrbitRight, OrbitAxis).GetSafeNormal();

	// 궤도 위의 위치
	FVector OrbitOffset = (OrbitRight * FMath::Cos(AngleRad) + OrbitForward * FMath::Sin(AngleRad)) * OrbitRadius;
	FVector DesiredLocation = OrbitCenter + OrbitOffset;

	// 보간으로 부드럽게
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime, FollowInterpSpeed);
	SetActorLocation(NewLocation);
}

void AKasaneBlade::TickAttack(float DeltaTime)
{
	switch (CurAttackPattern)
	{
	case EBladeAttackPattern::A1:
		TickAttackA1(DeltaTime);
		break;
	case EBladeAttackPattern::A2:
		// TODO
		break;
	case EBladeAttackPattern::A3:
		// TODO
		break;
	default:
		break;
	}
}

void AKasaneBlade::TickAttackA1(float DeltaTime)
{
	AttackElapsed += DeltaTime;

	FVector NewLocation = AttackOrigin + AttackDirection * AttackSpeed * AttackElapsed;
	SetActorLocation(NewLocation);

	float TraveledDistance = FVector::Dist(AttackOrigin, NewLocation);
	if (TraveledDistance >= AttackMaxDistance)
	{
		CurAttackPattern = EBladeAttackPattern::None;
		ChangeBladeState(EBladeState::Return);
	}
}

void AKasaneBlade::TickAttackA2(float DeltaTime)
{
}

void AKasaneBlade::TickAttackA3(float DeltaTime)
{
}

void AKasaneBlade::TickReturn(float DeltaTime)
{
	if (!OwnerActor.IsValid())
	{
		return;
	}

	ReturnElapsed += DeltaTime;
	float Alpha = FMath::Clamp(ReturnElapsed / ReturnDuration, 0.f, 1.f);

	// 끝점은 매 프레임 Owner 위치 추적
	// P1 = OwnerActor->GetActorLocation();
	P1 = OwnerActor->GetActorLocation() + ReturnTargetOffset;

	// 제어점: 시작~끝 중간 + 좌우 오프셋 + 높이 불규칙
	FVector MidPoint = (P0 + P1) * 0.5f;

	// 좌우 방향: 발사 방향에 수직
	FVector SideDir = FVector::CrossProduct(AttackDirection, FVector::UpVector).GetSafeNormal();

	// 블레이드마다 불규칙한 높이 오프셋 (BladeIndex 기반 시드)
	float HeightOffset = FMath::Sin(BladeIndex * 1.7f + 0.3f) * ReturnHeightVariance;

	CP0 = MidPoint
		+ SideDir * ReturnCurveSide * ReturnCurveStrength
		+ FVector(0.f, 0.f, HeightOffset);

	// Quadratic Bezier: P = (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
	float OneMinusT = 1.f - Alpha;
	FVector NewLocation = OneMinusT * OneMinusT * P0
		+ 2.f * OneMinusT * Alpha * CP0
		+ Alpha * Alpha * P1;

	SetActorLocation(NewLocation);

	// 도착 판정
	if (Alpha >= 1.f)
	{
		if (ReturnIdleIndex >= 0)
		{
			BladeIndex = ReturnIdleIndex;
			InitForIdle(IdleBladeCount);
		}
		else
		{
			ChangeBladeState(EBladeState::Inactive);
		}
	}
	// if (!OwnerActor.IsValid())
	// {
	// 	return;
	// }
	//
	// FVector TargetLocation = OwnerActor->GetActorLocation();
	// FVector CurrentLocation = GetActorLocation();
	//
	// FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	// FVector NewLocation = CurrentLocation + Direction * ReturnSpeed * DeltaTime;
	// SetActorLocation(NewLocation);
	//
	// // 도착 판정
	// float DistToOwner = FVector::Dist(NewLocation, TargetLocation);
	// if (DistToOwner < 100.f)
	// {
	// 	if (ReturnIdleIndex >= 0)
	// 	{
	// 		// Idle 블레이드로 복귀
	// 		ChangeBladeState(EBladeState::Idle);
	// 	}
	// 	else
	// 	{
	// 		ChangeBladeState(EBladeState::Inactive);
	// 	}
	// }
}

void AKasaneBlade::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurState != EBladeState::Attack)
	{
		return;
	}

	if (!IsValid(OtherActor) || OtherActor == OwnerActor.Get() || OtherActor == this)
	{
		return;
	}

	// 임시 데미지 수치 10
	DamageableHelpers::ApplyDamage(OtherActor, OwnerActor.Get(), 10);
}
