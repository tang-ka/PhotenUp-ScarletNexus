// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/BossCharacterBase.h"
#include "Boss/BossAIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Boss/BossAttackCollisionComponent.h"
 
ABossCharacterBase::ABossCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
 
	AIControllerClass = ABossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
 
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = 500.f;
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
	}
 
	bUseControllerRotationYaw = false;
	
	//콜리전(어택 발차기)

	AttackCollision = CreateDefaultSubobject<UBossAttackCollisionComponent>(TEXT("AttackCollision"));
	AttackCollision->SetupAttachment(GetMesh(), FName("Attack"));
	AttackCollision->SetSphereRadius(30.f);
	
	GlitchMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GlitchMesh"));
	GlitchMeshComp->SetupAttachment(GetMesh());
	GlitchMeshComp->SetLeaderPoseComponent(GetMesh());
	GlitchMeshComp->SetVisibility(false);
	GlitchMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlitchMeshComp->SetRenderInMainPass(true);
	GlitchMeshComp->SetCastShadow(false);
}
 
void ABossCharacterBase::BeginPlay()
{
	Super::BeginPlay();
 
	if (BossConfig)
	{
		InitializeWithConfig(BossConfig);
	}
}
 
void ABossCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDissolving)
	{
		DissolveTimer += DeltaTime;
		float Alpha = FMath::Clamp(DissolveTimer / DissolveDuration, 0.f, 1.f);
        
		// 사라지면 1->0, 나타나면 0->1
		float Opacity = bDissolveOut ? (1.f - Alpha) : Alpha;
		
		GetMesh()->SetScalarParameterValueOnMaterials(FName("Opacity"), Opacity);
		
		if (Alpha >= 1.f)
		{
			bDissolving = false;
		}
	}
}


void ABossCharacterBase::StartGlitchEffect()
{
	USkeletalMeshComponent* BossMesh = GetMesh();
	if (!BossMesh || !GlitchOverlayMaterial) return;

	GlitchMID = UMaterialInstanceDynamic::Create(GlitchOverlayMaterial, this);

	OriginalMaterials.Empty();
	for (int32 i = 0; i < BossMesh->GetNumMaterials(); i++)
	{
		OriginalMaterials.Add(BossMesh->GetMaterial(i));
		BossMesh->SetMaterial(i, GlitchMID);
	}

	UE_LOG(LogTemp, Warning, TEXT("[Boss] Glitch MID applied to %d slots"), 
		BossMesh->GetNumMaterials());
}

void ABossCharacterBase::StopGlitchEffect()
{
	USkeletalMeshComponent* BossMesh = GetMesh();
	if (!BossMesh) return;

	for (int32 i = 0; i < OriginalMaterials.Num(); i++)
	{
		BossMesh->SetMaterial(i, OriginalMaterials[i]);
	}
	OriginalMaterials.Empty();
	GlitchMID = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("[Boss] Glitch effect removed"));
}
 
void ABossCharacterBase::StartDissolve(float Duration, bool bOut)
{
	DissolveDuration = Duration;
	DissolveTimer = 0.f;
	bDissolving = true;
	bDissolveOut = bOut;
}

// IDamageable 구현

 
bool ABossCharacterBase::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	if (IDamageable::Execute_IsDead(this)) return false;
 
	if (CurrentHPValue <= 0.f)
	{
		return false;
	}
 
	const float OldHP = CurrentHPValue;
	const float DamageAmount = static_cast<float>(DamageInfo.DamageAmount);
	CurrentHPValue = FMath::Clamp(CurrentHPValue - DamageAmount, 0.f, MaxHPValue);
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 대미지: %d (HP: %.0f -> %.0f / %.0f)"),
		DamageInfo.DamageAmount, OldHP, CurrentHPValue, MaxHPValue);
 
	OnHPChanged.Broadcast(CurrentHPValue, MaxHPValue, DamageAmount);
	
	PlayDirectionalHitReaction(DamageInfo.DamageCauser);
	
	CheckPhaseTransition();
 
	if (CurrentHPValue <= 0.f)
	{
		HandleDeath();
	}
 
	return true;
}
 
int ABossCharacterBase::GetHP_Implementation() const
{
	return static_cast<int>(CurrentHPValue);
}
 
float ABossCharacterBase::GetHPPercent_Implementation() const
{
	if (MaxHPValue <= 0.f) return 0.f;
	return FMath::Clamp(CurrentHPValue / MaxHPValue, 0.f, 1.f);
}
 
bool ABossCharacterBase::IsDead_Implementation() const
{
	return CurrentHPValue <= 0.f;
}
 
 

// ICombatState 구현

 
bool ABossCharacterBase::IsAttacking_Implementation() const
{
	return CurrentCombatState == EBossCombatState::MeleeAttack
		|| CurrentCombatState == EBossCombatState::RangedAttack
		|| CurrentCombatState == EBossCombatState::Teleport;
}
 
bool ABossCharacterBase::HasSuperArmor_Implementation() const
{
	return false;
}
 
 

// 보스 전용

 
void ABossCharacterBase::InitializeWithConfig(UBossConfigDataAsset* Config)
{
	if (!Config)
	{
		UE_LOG(LogTemp, Error, TEXT("[Boss] InitializeWithConfig: Config가 nullptr입니다."));
		return;
	}
 
	BossConfig = Config;
	MaxHPValue = Config->MaxHP;
	CurrentHPValue = MaxHPValue;
	CurrentPhase = EBossPhase::Phase1;
 
	UE_LOG(LogTemp, Log, TEXT("[Boss] 초기화 완료 - HP: %.0f"), MaxHPValue);
}
 
 

// 내부 메서드

 
void ABossCharacterBase::CheckPhaseTransition()
{
	if (!BossConfig) return;
 
	const float HPRatio = GetHPPercent_Implementation();
	EBossPhase NewPhase = CurrentPhase;
 
	if (HPRatio <= 0.25f)
		NewPhase = EBossPhase::Phase3_Cutscene;
	else if (HPRatio <= 0.50f)
		NewPhase = EBossPhase::Phase2_Enhanced;
	else if (HPRatio <= 0.70f)
		NewPhase = EBossPhase::Phase2;
 
	if (NewPhase != CurrentPhase)
	{
		const EBossPhase OldPhase = CurrentPhase;
		CurrentPhase = NewPhase;
 
		UE_LOG(LogTemp, Warning, TEXT("[Boss] ========== 페이즈 전환! =========="));
		UE_LOG(LogTemp, Warning, TEXT("[Boss] %d → %d (HP: %.0f / %.0f, 비율: %.1f%%)"),
			static_cast<int32>(OldPhase),
			static_cast<int32>(NewPhase),
			CurrentHPValue, MaxHPValue, HPRatio * 100.f);
 
		OnPhaseChanged.Broadcast(OldPhase, NewPhase);
 
		if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
		{
			BossAI->SendStateTreeEvent(
				FGameplayTag::RequestGameplayTag(FName("Boss.Event.PhaseTransition")));
		}
	}
}
 
void ABossCharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[Boss] 사망 처리 시작"));

	// 이벤트만 전송 — 나머지는 Death State에서 처리
	if (ABossAIController* BossAI = Cast<ABossAIController>(GetController()))
	{
		BossAI->SendStateTreeEvent(
			FGameplayTag::RequestGameplayTag(FName("Boss.Event.Death")));
	}
	
}

void ABossCharacterBase::PlayDirectionalHitReaction(AActor* DamageCauser)
{
	if (!DamageCauser) return;

	// 공격 중이면 히트 리액션 스킵
	if (const UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		if (AnimInst->IsAnyMontagePlaying())
			return;
	}

	const FVector ToAttacker = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	const FVector Forward = GetActorForwardVector();
	const FVector Right = GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(Forward, ToAttacker);
	const float RightDot = FVector::DotProduct(Right, ToAttacker);

	UAnimMontage* HitMontage = nullptr;

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		HitMontage = (ForwardDot >= 0.f) ? HitReaction_Front : HitReaction_Back;
	}
	else
	{
		HitMontage = (RightDot >= 0.f) ? HitReaction_Right : HitReaction_Left;
	}

	if (HitMontage)
	{
		PlayAnimMontage(HitMontage);
	}
}


