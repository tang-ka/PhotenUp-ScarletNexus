// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKObject.h"

#include "ScarletNexus.h"
#include "Components/BoxComponent.h"
#include "FX/DissolveComponent.h"
#include "Interface/DamageableHelper.h"

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
	
	// 디졸브 콤포넌트
	DissolveComp = CreateDefaultSubobject<UDissolveComponent>(TEXT("DissolveComp"));
}

// Called when the game starts or when spawned
void APKObject::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false); 
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

	// 던지기 틸트 처리 — 목표 각도에 도달하면 각속도 제거
	if (bIsTilting)
	{
		if (GetActorQuat().Equals(ThrowTiltTargetQuat, 0.05f))
		{
			bIsTilting = false;
			BoxComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
		return;
	}
	
	if (ObjectState == EPKObjectState::CanBePickedUp && BoxComp->IsSimulatingPhysics())
	{
		if (BoxComp->GetPhysicsLinearVelocity().SizeSquared() < 10.f)
		{
			BoxComp->SetSimulatePhysics(false);
			BoxComp->SetEnableGravity(false);
			SetActorTickEnabled(false); // 정지 확인 후 다시 off
		}
	}
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
		BoxComp->SetNotifyRigidBodyCollision(true);
		BoxComp->SetEnableGravity(true);

		// 홀드 풀리면 다시 충돌 켜기
		BoxComp->SetCollisionProfileName(TEXT("PKObject"));
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// ObjectState = EPKObjectState::CanBePickedUp;
#if WITH_EDITOR
		PRINTLOG_GT(TEXT("PK오브젝트: %s 해제"), *this->GetName());
#endif
	}
}

void APKObject::OnPKThrown_Implementation(const FVector& ThrowDir, float ThrowForce)
{
	ObjectState = EPKObjectState::IsUsed;
	bUsedObject = true;

	// 목표 회전: WorldUp × ThrowDir 축의 -방향으로 TiltAngleDeg도
	const FVector RotationAxis = FVector::CrossProduct(FVector::UpVector, ThrowDir).GetSafeNormal();
	const FQuat TiltQuat = FQuat(RotationAxis, FMath::DegreesToRadians(-TiltAngleDeg));
	ThrowTiltTargetQuat = TiltQuat * GetActorQuat();

	// 물리 즉시 활성화 + 선형 속도
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetNotifyRigidBodyCollision(true);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetPhysicsLinearVelocity(ThrowDir * ThrowForce);

	// 현재 회전 → 목표 회전까지의 실제 각도 차이를 TiltDuration으로 나눠 각속도 동적 계산
	// (각도 차이 / 시간 = 도/초)
	const float AngleDiffDeg = FMath::RadiansToDegrees(GetActorQuat().AngularDistance(ThrowTiltTargetQuat));
	const float DynamicDegreesPerSec = (TiltDuration > KINDA_SMALL_NUMBER)
		? (AngleDiffDeg / TiltDuration)
		: AngleDiffDeg;
	BoxComp->SetPhysicsAngularVelocityInDegrees(RotationAxis * (-DynamicDegreesPerSec));

	bIsTilting = true;
	SetActorTickEnabled(true);

	// 최대 비행 시간 타이머 시작 — 타겟을 못 맞히고 멀리 날아가도 자동 디졸브
	GetWorld()->GetTimerManager().SetTimer(
		FlightTimerHandle,
		this,
		&APKObject::OnFlightTimeout,
		MaxFlightTime,
		false
	);
}

// 물리 적용 전용 던지기
void APKObject::OnPKThrownPS_Implementation(const FVector& ThrowDir, float ThrowForce)
{
	ObjectState = EPKObjectState::IsUsed;
	
	// 물리 활성화 
	BoxComp->SetSimulatePhysics(true);
	// 충돌 재활성화
	BoxComp->SetCollisionProfileName(TEXT("PKObject"));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bUsedObject = true;
	
	// 물리 충격
	//BoxComp->AddImpulse(ThrowDir * ThrowForce, NAME_None, true);
	// 물리 충격 커스텀 구현
	FVector impulse = ThrowDir * ThrowForce;
	
	// 질량 고려 : △V = Impulse / Mass
	/*float mass = BoxComp->GetMass();
	if (mass <= KINDA_SMALL_NUMBER) return;
	
	FVector deltaV = impulse / mass;*/
	BoxComp->SetPhysicsLinearVelocity(BoxComp->GetPhysicsLinearVelocity() + impulse);
}

void APKObject::OnFlightTimeout()
{
	if (ObjectState != EPKObjectState::IsUsed) return;

	if (DissolveComp)
	{
		DissolveComp->StartDissolve();
	}
}

void APKObject::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                         UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                         const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBoxHit - State: %s, NormalZ: %f"),
		*UEnum::GetValueAsString(ObjectState), Hit.ImpactNormal.Z);
	// CoolDown(릴리즈 후 낙하) 또는 IsUsed(던져진 후) 상태에서
	// 충돌 노말이 위쪽(바닥 또는 지면)을 향할 때 다시 집을 수 있는 상태로 복귀
	if (ObjectState == EPKObjectState::CoolDown )
	{
		// Hit.ImpactNormal.Z > 0.5f : 충돌면이 충분히 수평(바닥)에 가까울 때
		if (Hit.ImpactNormal.Z > 0.5f)
		{
			ObjectState = EPKObjectState::CanBePickedUp;
			bUsedObject = false;
			SetActorTickEnabled(true);
		}
	}
		
	// 사용되면 (던져짐) 부딪혔을 때 사라지게 함
	if (ObjectState == EPKObjectState::IsUsed)
	{
		// 바닥 충돌(수평면)만 무시 → 타겟까지 계속 날아감
		// 벽, 적 등 수직 방향 충돌에는 모두 디졸브
		if (Hit.ImpactNormal.Z > 0.5f)
		{
			return;
		}

		// 충돌 델리게이트 실행 — 데미지/카메라쉐이크/UI는 PsychokinesisComponent에서 처리
		OnPKObjectHit.ExecuteIfBound(this, OtherActor, OtherComp, Hit);

		if (DissolveComp)
		{
			GetWorld()->GetTimerManager().ClearTimer(FlightTimerHandle);
			DissolveComp->StartDissolve();
		}
	}

#if WITH_EDITOR
	// 디버그
	// if (DamageableHelpers::IsDamageable(OtherActor))
	// {
	// 	DrawDebugBox(
	// 		GetWorld(),
	// 		BoxComp->GetComponentLocation(),
	// 		BoxComp->GetScaledBoxExtent(),
	// 		BoxComp->GetComponentQuat(),
	// 		FColor::Magenta,
	// 		true,
	// 		2.f);
	// }
#endif
}
