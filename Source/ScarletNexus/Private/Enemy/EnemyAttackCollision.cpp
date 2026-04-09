// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttackCollision.h"

#include "ScarletNexus.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/DamageableHelper.h"


UEnemyAttackCollision::UEnemyAttackCollision()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyAttackCollision::BeginPlay()
{
	Super::BeginPlay();
	CreateCollisionComponent();
}

void UEnemyAttackCollision::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyAttackCollision::ActivateCollision()
{
	if (!CollisionComp) return;
	
	bIsActive = true;
	HitActorList.Empty();
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	// 활성화 시점에 이미 오버랩 중인 액터 처리
	TArray<AActor*> OverlappingActors;
	CollisionComp->GetOverlappingActors(OverlappingActors);
	for (AActor* actor : OverlappingActors)
	{
		TryApplyDamage(actor);
	}
}

void UEnemyAttackCollision::DeactivateCollision()
{
	if (!CollisionComp) return;
	
	bIsActive = false;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UEnemyAttackCollision::ClearHitActors()
{
	HitActorList.Empty();
}

void UEnemyAttackCollision::CreateCollisionComponent()
{
	AActor* owner = GetOwner();
	if (!owner) return;
	
	// 부착 대상 결정
	USceneComponent* attachParent = owner->GetRootComponent();
	if (AttachSocketName != NAME_None)
	{
		// 캐릭터의 메시 소켓에 부착
		if (ACharacter* character = Cast<ACharacter>(owner))
		{
			if (USkeletalMeshComponent* mesh = character->GetMesh())
			{
				attachParent = mesh;
			}
		}
	}
	
	switch (CollisionShape)
	{
	case EAttackCollisionShape::Sphere:
		{
			USphereComponent* sphere = NewObject<USphereComponent>(owner, TEXT("AttackCollision_Sphere"));
			sphere->SetSphereRadius(SphereRadius);
			CollisionComp = sphere;
		}
		break;
	case EAttackCollisionShape::Box:
		{
			UBoxComponent* box = NewObject<UBoxComponent>(owner, TEXT("AttackCollision_Box"));
			box->SetBoxExtent(BoxHalfExtent);
			CollisionComp = box;
		}
		break;
	}
	
	if (!CollisionComp) return;
	
	// 부착
	CollisionComp->SetupAttachment(attachParent, AttachSocketName);
	CollisionComp->RegisterComponent();
	CollisionComp->SetRelativeLocation(LocationOffset);
	
	// 콜리전 프로필 : 오버랩 전용
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본 비활성
	CollisionComp->SetCollisionProfileName(TEXT("EnemyAttack")); // EnemyAttack 이라는 이름의 프리셋
	CollisionComp->SetGenerateOverlapEvents(true);
	
	// 오버랩 이벤트 바인딩
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &UEnemyAttackCollision::OnOverlapBegin);
	
#if WITH_EDITOR
	CollisionComp->SetHiddenInGame(false); // 디버그용 에디터 시각화
#endif
}

// 오버랩 콜백
void UEnemyAttackCollision::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PRINTLOG_JJ(TEXT("피격"));
	if (!bIsActive) return;
	TryApplyDamage(OtherActor);
}

// 데미지 적용
void UEnemyAttackCollision::TryApplyDamage(AActor* TargetActor)
{
	if (!TargetActor) return;
	
	AActor* owner = GetOwner();
	// 자기 자신 무시
	if (TargetActor == owner) return;
	
	// 중복 히트 방지
	if (!bAllowMultiHitPerActivation && HitActorList.Contains(TargetActor)) return;
	
	// IDamageable 인터페이스 체크
	if (!DamageableHelpers::IsDamageable(TargetActor)) return;
	
	// 데미지 정보 구성
	FDamageInfo info;
	info.DamageAmount = DamageAmount;
	info.DamageCauser = owner;
	
	// 데미지 전달
	IDamageable::Execute_ReceiveDamage(TargetActor, info);
	
	// 히트 기록
	HitActorList.Add(TargetActor);
}
