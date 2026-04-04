// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// AI 자동 빙의
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();
	
	// 체력 바 위젯 콤포넌트
	HealthBarComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComp"));
	HealthBarComp->SetupAttachment(RootComponent);
	HealthBarComp->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthBarComp->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComp->SetDrawSize(FVector2D(120.f, 15.f));
	HealthBarComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 개체 이격
	if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
	{
		moveComp->bUseRVOAvoidance = true;
		moveComp->AvoidanceConsiderationRadius = 500.f;
		moveComp->AvoidanceWeight = 0.5f;
	}

	// 콜리전 프리셋 설정
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	CurrHP = MaxHP;
	
	// 위젯 클래스가 할당되어 있으면 세팅
	if (HPBarWidgetClass)
	{
		HealthBarComp->SetWidgetClass(HPBarWidgetClass);
	}
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::ApplyHitReaction(AActor* DamageCauser)
{
	if (bIsStunned || bIsDie) return;
	
	bIsStunned = true;
	
	// 이동 정지
	if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
	{
		moveComp->StopMovementImmediately();
	}
	
	// 히트 몽타주 재생
	if (HitReactionMontage)
	{
		if (UAnimInstance* animInst = GetMesh()->GetAnimInstance())
		{
			animInst->Montage_Play(HitReactionMontage, 1.f);
		}
	}
	
	// 스턴 타이머 
	GetWorldTimerManager().ClearTimer(StunTimerHandle);
	GetWorldTimerManager().SetTimer(
		StunTimerHandle,
		this,
		&AEnemyBase::OnHitStunEnd,
		HitStunDuration,
		false);
}

float AEnemyBase::GetHPRatio() const
{
	if (MaxHP <= 0) return 0.f;
	return static_cast<float>(CurrHP) / static_cast<float>(MaxHP);
}

void AEnemyBase::Die()
{
	if (bIsDie) return;
	bIsDie = true;
	
	// AI 정지
	if (AAIController* aic = Cast<AAIController>(GetController()))
	{
		aic->UnPossess();
	}
	
	// 충돌 비활성
	if (UCapsuleComponent* capsule = GetCapsuleComponent())
	{
		capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	// 이동 정지
	if (UCharacterMovementComponent* moveComp = GetCharacterMovement())
	{
		moveComp->DisableMovement();
	}
	
	// 체력바 숨기기
	if (HealthBarComp)
	{
		HealthBarComp->SetVisibility(false);
	}
	
	// Ragdoll
	if (bEnableRagdollOnDeath)
	{
		EnableRagdoll();
	}
	
	// EnemyManager에 사망 통보
	if (OwningManager)
	{
		OwningManager->OnEnemyDied(this);
	}
	else
	{
		// 타이머 종료 후 Destroy
		GetWorldTimerManager().SetTimer(
			DestroyTimerHandle,
			[this]() {Destroy();}, 
			DestroyDelay,
			false);
	}
}

void AEnemyBase::EnableRagdoll()
{
	if (USkeletalMeshComponent* meshComp = GetMesh())
	{
		meshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		meshComp->SetSimulatePhysics(true);
		meshComp->SetAllBodiesSimulatePhysics(true);
		meshComp->WakeAllRigidBodies();
	}
}

void AEnemyBase::OnHitStunEnd()
{
	bIsStunned = false;
}

bool AEnemyBase::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (bIsDie) return false;
	
	CurrHP = FMath::Clamp(CurrHP - DamageInfo.DamageAmount, 0, MaxHP);
	
	// 히트 리액션 플래그 설정
	if (IsAlive())
	{
		bIsStunned = true;
	}
	else
	{
		bIsDie = true;
	}
	return true;
}

int AEnemyBase::GetHP_Implementation() const
{
	return IDamageable::GetHP_Implementation();
}

float AEnemyBase::GetHPPercent_Implementation() const
{
	return IDamageable::GetHPPercent_Implementation();
}

bool AEnemyBase::IsDead_Implementation() const
{
	return IDamageable::IsDead_Implementation();
}
