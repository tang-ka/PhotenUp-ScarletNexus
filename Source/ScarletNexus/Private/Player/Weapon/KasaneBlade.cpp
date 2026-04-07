// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Weapon/KasaneBlade.h"

#include "ScarletNexus.h"
#include "Components/SphereComponent.h"
#include "Interface/DamageableHelper.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/WorldSettings.h"


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
		// TickAttack(DeltaTime);
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
	SetActiveCollision(bActivate);
	SetActorTickEnabled(bActivate);
}

void AKasaneBlade::SetActiveCollision(bool bActivate) const
{
	SphereComp->SetCollisionEnabled(bActivate ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AKasaneBlade::ChangeBladeState(EBladeState NewState)
{
	if (CurState == NewState)
	{
		return;
	}

	switch (NewState)
	{
	case EBladeState::Inactive:
		BeginInactive();
		break;
	case EBladeState::Idle:
		BeginIdle();
		break;
	case EBladeState::Attack:
		BeginAttack();
		break;
	case EBladeState::Return:
		BeginReturn();
		break;
	default:
		break;
	}

	// PRINTLOG_SH(TEXT("블레이드 %d 상태 변경: %s -> %s"), BladeIndex, *UEnum::GetValueAsString(CurState), *UEnum::GetValueAsString(NewState));
	
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
	PhaseOffset = (2.f * PI * BladeIndex) / IdleBladeCount;
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

void AKasaneBlade::BeginInactive()
{
	SetActive(false);
}

void AKasaneBlade::BeginIdle()
{
	SetActive(true);
}

void AKasaneBlade::BeginAttack()
{
	SetActive(true);
	GetRootComponent()->SetRelativeLocation(FVector::ZeroVector);
}

void AKasaneBlade::BeginReturn()
{
	ReturnElapsed = 0.f;
	P0 = GetActorLocation();

	// 구 위의 랜덤 오프셋을 BeginReturn에서 한 번만 결정
	FVector RandomDir = FMath::VRand(); // 구면 균등 랜덤 방향
	ReturnTargetOffset = RandomDir * ReturnArrivalRadius;
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
	FVector CurRelative = GetRootComponent()->GetRelativeLocation();
	FVector NewRelative = FMath::VInterpTo(CurRelative, FVector::ZeroVector, DeltaTime, 20);

	if (NewRelative.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		SetActorRelativeLocation(FVector::ZeroVector);
		return;
	}

	SetActorRelativeLocation(NewRelative);
}

// void AKasaneBlade::TickAttack(float DeltaTime)
// {
// 	switch (CurAttackPattern)
// 	{
// 	case EBladeAttackPattern::A1:
// 		TickAttackA1(DeltaTime);
// 		break;
// 	case EBladeAttackPattern::A2:
// 		// TODO
// 		break;
// 	case EBladeAttackPattern::A3:
// 		// TODO
// 		break;
// 	default:
// 		break;
// 	}
// }

// void AKasaneBlade::TickAttackA1(float DeltaTime)
// {
// 	AttackElapsed += DeltaTime;
//
// 	FVector NewLocation = AttackOrigin + AttackDirection * AttackSpeed * AttackElapsed;
// 	SetActorLocation(NewLocation);
//
// 	float TraveledDistance = FVector::Dist(AttackOrigin, NewLocation);
// 	if (TraveledDistance >= AttackMaxDistance)
// 	{
// 		CurAttackPattern = EBladeAttackPattern::None;
// 		ChangeBladeState(EBladeState::Return);
// 	}
// }

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
}

void AKasaneBlade::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == OwnerActor.Get() || OtherActor == this)
	{
		return;
	}

	// 임시 데미지 수치 10
	const bool bHit = DamageableHelpers::ApplyDamage(OtherActor, OwnerActor.Get(), 10);
	if (bHit)
	{
		TriggerHitStop();
		OnBladeHit.ExecuteIfBound(this, OtherActor); // 충돌 델리게이트 호출
	}
}

void AKasaneBlade::TriggerHitStop()
{
	if (!GetWorld())
	{
		return;
	}

	// 이미 히트스탑 진행 중이면 타이머만 갱신
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), HitStopTimeDilation);

	// TimeDilation이 적용된 게임 시간 기준으로 타이머를 역산
	// 타이머도 딜레이션 영향을 받으므로, 실제로 원하는 실시간을 얻으려면 곱해야 함
	// 예) 실제 0.08초 → 게임 타이머로 0.08 * 0.05 = 0.004 게임초 → 실시간 0.08초
	const float AdjustedDuration = (HitStopTimeDilation > KINDA_SMALL_NUMBER)
		? HitStopDuration * HitStopTimeDilation
		: HitStopDuration;

	GetWorldTimerManager().SetTimer(
		HitStopTimerHandle,
		this,
		&AKasaneBlade::EndHitStop,
		AdjustedDuration,
		false
	);
}

void AKasaneBlade::EndHitStop()
{
	if (!GetWorld())
	{
		return;
	}
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}
