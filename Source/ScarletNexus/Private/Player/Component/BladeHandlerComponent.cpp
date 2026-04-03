// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/BladeHandlerComponent.h"

#include "Player/Weapon/KasaneBlade.h"


UBladeHandlerComponent::UBladeHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// 블레이드 클래스 설정 (예시)
	static ConstructorHelpers::FClassFinder<AKasaneBlade> BladeBPClass(TEXT("/Game/SSH/Blueprints/BP_KasaneBlade"));
	if (BladeBPClass.Succeeded())
	{
		BladeClass = BladeBPClass.Class;
	}
}

void UBladeHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnBladePool();
	ActivateIdleBlades();
}

void UBladeHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBladeHandlerComponent::ExecuteAttackA1()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector Origin = Owner->GetActorLocation();
	FVector OwnerForward = Owner->GetActorForwardVector();
	int32 BladeCount = BladePool.Num();

	// 부채꼴 범위: 정면 기준 좌우 대칭
	// ex) SpreadAngle = 120 → -60도 ~ +60도
	float HalfSpread = A1SpreadAngle * 0.5f;
	
	for (int32 i = 0; i < BladeCount; ++i)
	{
		if (!BladePool[i])
		{
			continue;
		}
		
		// -HalfSpread ~ +HalfSpread 균등 분배
		float AngleDeg = 0.f;
		if (BladeCount == 1)
		{
			AngleDeg = 0.f;  // 정면
		}
		else
		{
			AngleDeg = -HalfSpread + (A1SpreadAngle * i / (BladeCount - 1));
		}

		FVector Direction = OwnerForward.RotateAngleAxis(AngleDeg, FVector::UpVector);
		// 랜덤 높이 오프셋 추가
		float HeightOffset = FMath::FRandRange(0.f, 20.f);
		Origin.Z += HeightOffset;

		BladePool[i]->ReturnIdleIndex = (i < IdleBladeCount) ? i : -1;
		BladePool[i]->LaunchAttack(EBladeAttackPattern::A1, Origin, Direction, A1MaxDistance, A1Speed);
	}
}

void UBladeHandlerComponent::SpawnBladePool()
{
	UWorld* World = GetWorld();
	if (!World || !BladeClass)
	{
		return;
	}
	
	for (int32 i = 0; i < PoolSize; ++i)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AKasaneBlade* Blade = World->SpawnActor<AKasaneBlade>(
			BladeClass,
			GetOwner()->GetActorLocation(),
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (Blade)
		{
			// 초기 상태: 비활성
			Blade->Init(GetOwner(), i);
			Blade->SetActive(false);
			BladePool.Add(Blade);
		}
	}
}

void UBladeHandlerComponent::ActivateIdleBlades()
{	
	int32 Count = FMath::Min(IdleBladeCount, BladePool.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		BladePool[i]->InitForIdle(Count);
	}
}

