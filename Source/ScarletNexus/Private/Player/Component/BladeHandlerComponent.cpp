// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/BladeHandlerComponent.h"

#include "Player/Weapon/KasaneBlade.h"
#include "Interface/DamageableHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Component/ComboComponent.h"
#include "Player/Component/PlayerPerceptionComponent.h"
#include "Player/Component/PsychokinesisComponent.h"
#include "Data/ComboAttackDataAsset.h"


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
	// ActivateIdleBlades();
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
			AngleDeg = 0.f; // 정면
		}
		else
		{
			AngleDeg = -HalfSpread + (A1SpreadAngle * i / (BladeCount - 1));
		}

		FVector Direction = OwnerForward.RotateAngleAxis(AngleDeg, FVector::UpVector);
		// 랜덤 높이 오프셋 추가
		// float HeightOffset = FMath::FRandRange(0.f, 4.f);
		// Direction.Z += HeightOffset;
		// Direction.Normalize();

		BladePool[i]->ReturnIdleIndex = (i < IdleBladeCount) ? i : -1;
		BladePool[i]->LaunchAttack(EBladeAttackPattern::A1, Origin, Direction, A1MaxDistance, A1Speed);
	}
}

void UBladeHandlerComponent::SetActiveAllBladesCollision(bool bActivate)
{
	for (AKasaneBlade* Blade : BladePool)
	{
		if (Blade)
		{
			Blade->SetActiveCollision(bActivate);
		}
	}
}

void UBladeHandlerComponent::SetCanCriticalAllBlades(bool bCanCritical)
{
	for (AKasaneBlade* Blade : BladePool)
	{
		if (Blade)
		{
			Blade->SetCanCritical(bCanCritical);
		}
	}
}

void UBladeHandlerComponent::SetAttackStateAllBlades()
{
	for (AKasaneBlade* Blade : BladePool)
	{
		if (Blade)
		{
			Blade->ChangeBladeState(EBladeState::Attack);
		}
	}
}

void UBladeHandlerComponent::SetDefaultStateAllBlades()
{
	for (AKasaneBlade* Blade : BladePool)
	{
		if (Blade)
		{
			Blade->ChangeBladeState(Blade->GetDefaultState());
		}
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

		USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(
			GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (Blade)
		{
			FName BoneName = FName(*FString::Printf(TEXT("Weapon%02d"), i + 1));

			Blade->AttachToComponent(
				SkelMesh,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				BoneName
			);

			// 초기 상태: 비활성
			Blade->Init(GetOwner(), i);
			if (i < IdleBladeCount)
			{
				Blade->SetDefaultState(EBladeState::Idle);
				Blade->InitForIdle(IdleBladeCount);
			}
			else
			{
				Blade->SetDefaultState(EBladeState::Inactive);
			}
			Blade->ChangeBladeState(Blade->GetDefaultState());

			// 충돌 델리게이트 바인딩
			Blade->OnBladeHit.BindUObject(this, &UBladeHandlerComponent::HandleBladeHit);

			BladePool.Add(Blade);
		}
	}
}

void UBladeHandlerComponent::ActivateIdleBlades()
{
	int32 Count = FMath::Min(IdleBladeCount, BladePool.Num());
	Count = BladePool.Num();
	for (int32 i = 0; i < Count; ++i)
	{
		BladePool[i]->InitForIdle(Count);
	}
}

void UBladeHandlerComponent::HandleBladeHit(AKasaneBlade* HitBlade, UPrimitiveComponent* OverlappedComponent,
                                            AActor* HitActor, UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!GetWorld() || !IsValid(HitActor))
	{
		return;
	}

	// =============================================
	// 1. 콤보 데이터에서 데미지 수치 획득
	// =============================================
	int32 DamageAmount = 0;
	if (APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(GetOwner()))
	{
		if (UComboComponent* Combo = Player->GetComboComp())
		{
			if (const UComboAttackDataAsset* CurrentAttack = Combo->GetCurrentAttack())
			{
				const FAttackBalanceData BalanceData = CurrentAttack->GetBalanceData();
				DamageAmount = FMath::RoundToInt(BalanceData.BaseDamage);
			}
		}
	}

	// =============================================
	// 2. 데미지 유효 검사 후 적용
	// =============================================
	if (!DamageableHelpers::IsDamageable(HitActor))
	{
		return;
	}

	const bool bHit = DamageableHelpers::ApplyDamage(HitActor, GetOwner(), DamageAmount);
	if (!bHit)
	{
		return;
	}

	// =============================================
	// 3. HitStop (Blade 내부 처리)
	// =============================================
	if (HitBlade)
	{
		HitBlade->TriggerHitStop();
	}

	// =============================================
	// 4. 데미지 위젯 데이터 브로드캐스트
	// =============================================
	FDamageWidgetData WidgetData;
	WidgetData.DamageAmount = DamageAmount;
	
	if (bFromSweep)
	{
		WidgetData.WorldLocation = SweepResult.ImpactPoint;
	}
	else
	{

		OtherComp->GetClosestPointOnCollision(HitBlade->GetActorLocation(), WidgetData.WorldLocation);
	}

	OnBladeDamageDealt.Broadcast(WidgetData);

	// =============================================
	// 5. CameraShake 재생
	// =============================================
	if (HitCameraShakeClass)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->ClientStartCameraShake(HitCameraShakeClass);
		}
	}

	// =============================================
	// 6. 락온 타겟 설정 (아직 락온 중이 아닐 때만)
	// =============================================
	if (APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(GetOwner()))
	{
		UPlayerPerceptionComponent* Perception = Player->GetPerceptionComp();
		if (!Perception->IsLockedOnActivate())
		{
			Perception->LockOnToTarget(HitActor);
		}

		// =============================================
		// 7. 기본공격 히트 → 염동력 콤보 StrongThrow 플래그
		// =============================================
		if (UComboComponent* Combo = Player->GetComboComp())
		{
			if (const UComboAttackDataAsset* CurAttack = Combo->GetCurrentAttack())
			{
				if (CurAttack->AttackType == EAttackType::BasicAttack)
				{
					Player->GetPsychokinesisComp()->SetBasicAttackHitConfirmed(true);
				}
			}
		}
	}
}
