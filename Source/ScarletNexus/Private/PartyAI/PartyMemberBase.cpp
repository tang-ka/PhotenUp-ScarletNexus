// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyMemberBase.h"

#include "ScarletNexus.h"
#include "Components/SphereComponent.h"
#include "Interface/DamageableHelper.h"
#include "Kismet/GameplayStatics.h"
#include "PartyAI/PartyAIComponent.h"
#include "PartyAI/PartyAIController.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Component/PartyHandlerComponent.h"

// Sets default values
APartyMemberBase::APartyMemberBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = APartyAIController::StaticClass();
	
	PartyAIComp = CreateDefaultSubobject<UPartyAIComponent>(TEXT("PartyAIComp"));
	StateTreeComp = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComp"));
	
	// 자동 시작 끄기 - StartAI()에서 명시적으로 시작함
	StateTreeComp->SetStartLogicAutomatically(false);
	
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(300.f);
	InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void APartyMemberBase::BeginPlay()
{
	Super::BeginPlay();
	CurrHP = MaxHP;
	// 초기 HP 브로드캐스트
	OnHPChanged.Broadcast(CurrHP, MaxHP);
	
	// 트리거 바인딩
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &APartyMemberBase::OnInteractionBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &APartyMemberBase::OnInteractionEndOverlap);
	
	PRINTLOG_GT(TEXT("Controller: %s"), GetController() ? *GetController()->GetName() : TEXT("Null"));
}

// Called every frame
void APartyMemberBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 상태 디버그
#if WITH_EDITOR
	
#endif
	
}

// Called to bind functionality to input
void APartyMemberBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 스탯
void APartyMemberBase::TakeDamage_Party(int Damage)
{
	if (!IsAlive()) return;
	CurrHP = FMath::Clamp(CurrHP - Damage, 0, MaxHP);
	OnHPChanged.Broadcast(CurrHP, MaxHP);
	
	// 데미지 감소 후 HP가 0 이하면 사망 처리
	if (CurrHP <= 0)
	{
		
	}
}

// 쿨타임
void APartyMemberBase::SetCooldown(FName SkillName, float Duration)
{
	float now = GetWorld()->GetTimeSeconds();
	CooldownEndTimes.Add(SkillName, now + Duration);
}

bool APartyMemberBase::IsSkillReady(FName SkillName) const
{
	const float* endTime = CooldownEndTimes.Find(SkillName);
	if (!endTime) return true; // 아직 한번도 사용하지 않은 상태
	
	float now = GetWorld()->GetTimeSeconds();
	return now >= *endTime;
}

float APartyMemberBase::GetRemainCooldown(FName SkillName) const
{
	const float* endTime = CooldownEndTimes.Find(SkillName);
	if (!endTime) return 0;
	
	float now = GetWorld()->GetTimeSeconds();
	return FMath::Max(0, *endTime - now);
}

float APartyMemberBase::PlayMontage(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage) return 0.f;
	UAnimInstance* anim = GetMesh()->GetAnimInstance();
	if (!anim) return 0.f;
	return anim->Montage_Play(Montage, PlayRate);
}

void APartyMemberBase::StopMontage(UAnimMontage* Montage, float BlendOutTime)
{
	UAnimInstance* anim = GetMesh()->GetAnimInstance();
	if (anim) anim->Montage_Stop(BlendOutTime, Montage);
}

// 데미지 전달
void APartyMemberBase::ApplyDamageToHitTarget(AActor* HitTarget, AActor* Causer, int CauserATK)
{
	int finalATK = ATK + CauserATK;
	DamageableHelpers::ApplyDamage(HitTarget, Causer, finalATK);
}

void APartyMemberBase::ActivatePartyAI()
{
	if (bPartyActive) return;
	bPartyActive = true;
	
	// Controller가 없으면 스폰 -> OnPossess -> StartAI 자동 호출
	if (!GetController())
	{
		SpawnDefaultController();
	}
}

void APartyMemberBase::DeactivatePartyAI()
{
	if (!bPartyActive) return;
	bPartyActive = false;
	
	// StateTree 정지
	if (StateTreeComp)
	{
		StateTreeComp->StopLogic(TEXT("Party Removed"));
	}
	
	// AIController 분리
	if (AController* con = GetController())
	{
		con->UnPossess();
		con->Destroyed();
	}
}

void APartyMemberBase::OnInteract(APlayerCharacterBase* Requester)
{
	if (bPartyActive) return;
	
	// 여기서 대화 UI 띄우기 또는 바로 합류 처리
	// 즉시 합류
	if (UPartyHandlerComponent* Handler = Requester->FindComponentByClass<UPartyHandlerComponent>())
	{
		if (Handler->AddPartyMember(this))
		{
			bPlayerInRange = false;
			InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void APartyMemberBase::OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* player = Cast<APlayerCharacterBase>(OtherActor);
	if (player)
	{
		bPlayerInRange = true;
		// TODO: F키로 말걸기 UI 표시
		// 현재는 즉시 합류
		OnInteract(player);
	}
}

void APartyMemberBase::OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacterBase>(OtherActor))
	{
		bPlayerInRange = false;
		// TODO UI Hide
	}
}

void APartyMemberBase::PlayJoinPartySound()
{
	if (JoinPartySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, JoinPartySound, GetActorLocation());
	}
}
