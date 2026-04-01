// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossAttackExecutor.h"
#include "Boss/BossCloneActor.h"
#include "Boss/BossCharacterBase.h"
#include "Interface/DamageableHelper.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Interface/Damageable.h"

 

// EnterState

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterState(
    FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& Data = Context.GetInstanceData(*this);
    Data.PhaseTimer = 0.f;
    Data.bDamageApplied = false;

    ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
    if (!Boss) return EStateTreeRunStatus::Failed;

    if (IDamageable::Execute_IsDead(Boss))
    {
        UE_LOG(LogTemp, Log, TEXT("[AttackExecutor] 보스 사망 상태 — 공격 중지"));
        return EStateTreeRunStatus::Running;
    }

    Data.ActiveAttack = SelectAttack(Boss);
	
	if (BossConfig)
	{
		const ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(Boss);
		const EBossPhase Phase = BossChar ? BossChar->GetCurrentPhase() : EBossPhase::Phase1;
		TArray<FBossAttackPattern> Patterns = BossConfig->GetAvailablePatterns(Phase);
    
		for (int32 i = 0; i < Patterns.Num(); i++)
		{
			if (Patterns[i].AttackType == ToDataAssetType(Data.ActiveAttack))
			{
				Data.SelectedPatternIndex = i;
				if (Patterns[i].AttackMontage)
				{
					Boss->PlayAnimMontage(Patterns[i].AttackMontage);
				}
				break;
			}
		}
	}

    //  DataAsset 몽타주 재생
    if (BossConfig)
    {
        const ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(Boss);
        const EBossPhase Phase = BossChar ? BossChar->GetCurrentPhase() : EBossPhase::Phase1;
        TArray<FBossAttackPattern> Patterns = BossConfig->GetAvailablePatterns(Phase);
        
        // 현재 선택된 공격 타입에 맞는 패턴 찾기
        for (int32 i = 0; i < Patterns.Num(); i++)
        {
            if (Patterns[i].AttackType == ToDataAssetType(Data.ActiveAttack))
            {
                Data.SelectedPatternIndex = i;
                if (Patterns[i].AttackMontage)
                {
                    Boss->PlayAnimMontage(Patterns[i].AttackMontage);
                }
                break;
            }
        }
    }

    switch (Data.ActiveAttack)
    {
    case EActiveAttackType::TeleportKick:    return EnterTeleportKick(Data, Boss);
    case EActiveAttackType::CloneRush:       return EnterCloneRush(Data, Boss);
    case EActiveAttackType::AerialElectric:  return EnterAerialElectric(Data, Boss);
    case EActiveAttackType::IceSpikes:       return EnterIceSpikes(Data, Boss);
    case EActiveAttackType::ElectricOrbs:    return EnterElectricOrbs(Data, Boss);
    default: return EStateTreeRunStatus::Failed;
    }
}
 

// Tick

EStateTreeRunStatus FSTTask_BossAttackExecutor::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;
 
	
	if (IDamageable::Execute_IsDead(Boss))
{
    return EStateTreeRunStatus::Running;
}

// 각 공격 패턴 Tick 실행
EStateTreeRunStatus Result = EStateTreeRunStatus::Running;
switch (Data.ActiveAttack)
{
case EActiveAttackType::TeleportKick:    Result = TickTeleportKick(Data, Boss, DeltaTime); break;
case EActiveAttackType::CloneRush:       Result = TickCloneRush(Data, Boss, DeltaTime); break;
case EActiveAttackType::AerialElectric:  Result = TickAerialElectric(Data, Boss, DeltaTime); break;
case EActiveAttackType::IceSpikes:       Result = TickIceSpikes(Data, Boss, DeltaTime); break;
case EActiveAttackType::ElectricOrbs:    Result = TickElectricOrbs(Data, Boss, DeltaTime); break;
default: return EStateTreeRunStatus::Failed;
}

// 공격 로직은 끝났지만 몽타주가 아직 재생 중이면 대기
if (Result == EStateTreeRunStatus::Succeeded)
{
    if (const UAnimInstance* AnimInst = Boss->GetMesh()->GetAnimInstance())
    {
        if (AnimInst->IsAnyMontagePlaying())
        {
            return EStateTreeRunStatus::Running;
        }
    }
}

return Result;
}
 

// ExitState

void FSTTask_BossAttackExecutor::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	if (ACharacter* Boss = Cast<ACharacter>(Data.ContextActor))
	{
		Boss->SetActorHiddenInGame(false);
		Boss->SetActorEnableCollision(true);

		// 몽타주 정지 — Slot 해제되어야 Walk로 돌아감
		if (UAnimInstance* AnimInst = Boss->GetMesh()->GetAnimInstance())
		{
			AnimInst->StopAllMontages(0.25f);  // 0.25초 블렌드아웃
		}
	}
	if (Data.LeftClone && !Data.LeftClone->IsActorBeingDestroyed()) Data.LeftClone->Destroy();
	if (Data.RightClone && !Data.RightClone->IsActorBeingDestroyed()) Data.RightClone->Destroy();
	Data.LeftClone = nullptr;
	Data.RightClone = nullptr;
}
 
EActiveAttackType FSTTask_BossAttackExecutor::ToActiveType(EBossAttackType Type)
{
	switch (Type)
	{
	case EBossAttackType::TeleportKick:    return EActiveAttackType::TeleportKick;
	case EBossAttackType::CloneRush:       return EActiveAttackType::CloneRush;
	case EBossAttackType::AerialElectric:  return EActiveAttackType::AerialElectric;
	case EBossAttackType::IceSpikes:       return EActiveAttackType::IceSpikes;
	case EBossAttackType::ElectricOrbs:    return EActiveAttackType::ElectricOrbs;
	case EBossAttackType::TelekinesisThrow:return EActiveAttackType::TelekinesisThrow;
	default:                               return EActiveAttackType::None;
	}
}

EBossAttackType FSTTask_BossAttackExecutor::ToDataAssetType(EActiveAttackType Type)
{
	switch (Type)
	{
	case EActiveAttackType::TeleportKick:    return EBossAttackType::TeleportKick;
	case EActiveAttackType::CloneRush:       return EBossAttackType::CloneRush;
	case EActiveAttackType::AerialElectric:  return EBossAttackType::AerialElectric;
	case EActiveAttackType::IceSpikes:       return EBossAttackType::IceSpikes;
	case EActiveAttackType::ElectricOrbs:    return EBossAttackType::ElectricOrbs;
	case EActiveAttackType::TelekinesisThrow:return EBossAttackType::TelekinesisThrow;
	default:                                 return EBossAttackType::TeleportKick;
	}
}


// 공격 선택 (페이즈별)

EActiveAttackType FSTTask_BossAttackExecutor::SelectAttack(const ACharacter* Boss) const
{
    const ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(Boss);
    const EBossPhase Phase = BossChar ? BossChar->GetCurrentPhase() : EBossPhase::Phase1;

    // DataAsset 가중치 기반
    if (BossConfig)
    {
        TArray<FBossAttackPattern> Patterns = BossConfig->GetAvailablePatterns(Phase);
        if (Patterns.Num() > 0)
        {
            float DistToPlayer = 0.f;
            if (const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(
                    Boss->GetWorld(), 0))
            {
                DistToPlayer = FVector::Dist(Boss->GetActorLocation(),
                    Player->GetActorLocation());
            }

            TArray<float> Weights;
            float TotalWeight = 0.f;
            for (const FBossAttackPattern& P : Patterns)
            {
                float W = P.SelectionWeight;
                if (P.AttackRange <= 300.f)
                    W *= (DistToPlayer <= P.AttackRange * 1.2f) ? 2.f : 0.2f;
                else if (DistToPlayer > 400.f)
                    W *= 1.5f;
                Weights.Add(W);
                TotalWeight += W;
            }

            float Roll = FMath::FRandRange(0.f, TotalWeight);
            float Acc = 0.f;
            for (int32 i = 0; i < Weights.Num(); i++)
            {
                Acc += Weights[i];
                if (Roll <= Acc)
                    return ToActiveType(Patterns[i].AttackType);
            }
            return ToActiveType(Patterns.Last().AttackType);
        }
    }

    // Fallback: DataAsset 없으면 기존 균등 랜덤
    TArray<EActiveAttackType> Pool;
    Pool.Add(EActiveAttackType::TeleportKick);
    Pool.Add(EActiveAttackType::CloneRush);
    Pool.Add(EActiveAttackType::AerialElectric);
    Pool.Add(EActiveAttackType::IceSpikes);
    if (Phase >= EBossPhase::Phase2)
        Pool.Add(EActiveAttackType::ElectricOrbs);

    return Pool[FMath::RandRange(0, Pool.Num() - 1)];
}

// 텔레포트 킥

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterTeleportKick(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player) return EStateTreeRunStatus::Failed;
	Data.TKPhase = ETKPhase::Vanishing;
	const FVector PlayerLoc = Player->GetActorLocation();
	FVector Target = PlayerLoc - Player->GetActorForwardVector() * TK_TeleportOffset;
	if (const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Boss->GetWorld()))
	{
		FNavLocation NavLoc;
		if (NavSys->ProjectPointToNavigation(Target, NavLoc, FVector(300.f))) Target = NavLoc.Location;
	}
	Data.TKTarget = Target;
	Boss->SetActorHiddenInGame(true);
	Boss->SetActorEnableCollision(false);
	UE_LOG(LogTemp, Log, TEXT("[TeleportKick] 시작"));
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossAttackExecutor::TickTeleportKick(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
	switch (Data.TKPhase)
	{
	case ETKPhase::Vanishing:
		if (Data.PhaseTimer >= TK_VanishDuration)
		{
			Boss->SetActorLocation(Data.TKTarget);
			if (const ACharacter* P = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0))
			{ FVector D = (P->GetActorLocation() - Data.TKTarget).GetSafeNormal(); if (!D.IsNearlyZero()) Boss->SetActorRotation(D.Rotation()); }
			Data.TKPhase = ETKPhase::Teleporting; Data.PhaseTimer = 0.f;
		} break;
	case ETKPhase::Teleporting:
		if (Data.PhaseTimer >= TK_AppearDelay)
		{
			Boss->SetActorHiddenInGame(false); Boss->SetActorEnableCollision(true);
			Data.TKPhase = ETKPhase::Kicking; Data.PhaseTimer = 0.f;
		} break;
	case ETKPhase::Kicking:
		if (!Data.bDamageApplied)
		{
			ApplyDamageInRadius(Boss, Boss->GetActorLocation() + Boss->GetActorForwardVector() * (TK_KickRadius * 0.5f),
				TK_KickRadius, TK_Damage, TK_KnockbackForce, Boss->GetActorForwardVector());
			Data.bDamageApplied = true;
		}
		if (Data.PhaseTimer >= TK_KickDuration) return EStateTreeRunStatus::Succeeded;
		break;
	default: return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}
 

// 분신 돌진

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterCloneRush(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player) return EStateTreeRunStatus::Failed;
	Data.CRPhase = ECRPhase::SpawnClones; Data.bDamageApplied = false; Data.bBossRushDamageApplied = false;
	Data.LeftClone = nullptr; Data.RightClone = nullptr;
	Data.CRStartLocation = Boss->GetActorLocation();
	const FVector ToP = Player->GetActorLocation() - Data.CRStartLocation;
	Data.CRDirection = FVector(ToP.X, ToP.Y, 0.f).GetSafeNormal();
	const float DistToPlayer = FVector::Dist2D(Data.CRStartLocation, Player->GetActorLocation());
	Data.CRTargetLocation = Data.CRStartLocation + Data.CRDirection * (DistToPlayer + 200.f);
	Boss->SetActorRotation(Data.CRDirection.Rotation());
	const FVector RV = FVector(-Data.CRDirection.Y, Data.CRDirection.X, 0.f);
	FActorSpawnParameters SP; SP.Owner = Boss; SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 
	// 보스 캡슐/메시 정보 캐싱
	float BossCapsuleRadius = 42.f, BossCapsuleHalfHeight = 96.f;
	if (const UCapsuleComponent* BossCapsule = Boss->GetCapsuleComponent())
	{
		BossCapsule->GetScaledCapsuleSize(BossCapsuleRadius, BossCapsuleHalfHeight);
	}
 
	const FVector LL = Data.CRStartLocation - RV * CR_CloneSpacing;
	if (ABossCloneActor* LC = Boss->GetWorld()->SpawnActor<ABossCloneActor>(ABossCloneActor::StaticClass(), LL, FRotator::ZeroRotator, SP))
	{
		FVector LD = FVector(Player->GetActorLocation().X-LL.X, Player->GetActorLocation().Y-LL.Y, 0.f).GetSafeNormal();
		float LDist = FVector::Dist2D(LL, Player->GetActorLocation()) + 200.f;
		LC->InitRush(LD, CR_RushSpeed, LDist, CR_Damage, CR_RushWidth, CR_KnockbackForce);
 
		// 보스 외형 복사 (메시 + 스케일 + 캡슐)
		if (auto* BM = Boss->GetMesh())
		{
			if (auto* CM = LC->FindComponentByClass<USkeletalMeshComponent>())
			{
				CM->SetSkeletalMesh(BM->GetSkeletalMeshAsset());
				CM->SetRelativeTransform(BM->GetRelativeTransform());
				CM->SetAnimInstanceClass(BM->GetAnimInstance() ? BM->GetAnimInstance()->GetClass() : nullptr);
			}
		}
		if (auto* CC = LC->FindComponentByClass<UCapsuleComponent>())
		{
			CC->SetCapsuleSize(BossCapsuleRadius, BossCapsuleHalfHeight);
		}
		LC->SetActorScale3D(Boss->GetActorScale3D());
 
		Data.LeftClone = LC;
	}
	const FVector RL = Data.CRStartLocation + RV * CR_CloneSpacing;
	if (ABossCloneActor* RC = Boss->GetWorld()->SpawnActor<ABossCloneActor>(ABossCloneActor::StaticClass(), RL, FRotator::ZeroRotator, SP))
	{
		FVector RD = FVector(Player->GetActorLocation().X-RL.X, Player->GetActorLocation().Y-RL.Y, 0.f).GetSafeNormal();
		float RDist = FVector::Dist2D(RL, Player->GetActorLocation()) + 200.f;
		RC->InitRush(RD, CR_RushSpeed, RDist, CR_Damage, CR_RushWidth, CR_KnockbackForce);
 
		// 보스 외형 복사
		if (auto* BM = Boss->GetMesh())
		{
			if (auto* CM = RC->FindComponentByClass<USkeletalMeshComponent>())
			{
				CM->SetSkeletalMesh(BM->GetSkeletalMeshAsset());
				CM->SetRelativeTransform(BM->GetRelativeTransform());
				CM->SetAnimInstanceClass(BM->GetAnimInstance() ? BM->GetAnimInstance()->GetClass() : nullptr);
			}
		}
		if (auto* CC = RC->FindComponentByClass<UCapsuleComponent>())
		{
			CC->SetCapsuleSize(BossCapsuleRadius, BossCapsuleHalfHeight);
		}
		RC->SetActorScale3D(Boss->GetActorScale3D());
 
		Data.RightClone = RC;
	}
	UE_LOG(LogTemp, Log, TEXT("[CloneRush] 3체 준비 완료"));
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossAttackExecutor::TickCloneRush(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
	switch (Data.CRPhase)
	{
	case ECRPhase::SpawnClones: Data.CRPhase = ECRPhase::WindUp; Data.PhaseTimer = 0.f; break;
	case ECRPhase::WindUp:
		if (Data.PhaseTimer >= CR_WindUpDuration) { if (auto* LC = Cast<ABossCloneActor>(Data.LeftClone)) LC->StartRush(); Data.CRPhase = ECRPhase::LeftRush; Data.PhaseTimer = 0.f; } break;
	case ECRPhase::LeftRush:
		{ auto* LC = Cast<ABossCloneActor>(Data.LeftClone); if (!LC || LC->IsRushComplete()) { Data.CRPhase = ECRPhase::LeftDelay; Data.PhaseTimer = 0.f; } } break;
	case ECRPhase::LeftDelay:
		if (Data.PhaseTimer >= CR_SequenceDelay) { if (auto* RC = Cast<ABossCloneActor>(Data.RightClone)) RC->StartRush(); Data.CRPhase = ECRPhase::RightRush; Data.PhaseTimer = 0.f; } break;
	case ECRPhase::RightRush:
		{ auto* RC = Cast<ABossCloneActor>(Data.RightClone); if (!RC || RC->IsRushComplete()) { Data.CRPhase = ECRPhase::RightDelay; Data.PhaseTimer = 0.f; } } break;
	case ECRPhase::RightDelay:
		if (Data.PhaseTimer >= CR_SequenceDelay) { Data.CRPhase = ECRPhase::BossRush; Data.PhaseTimer = 0.f; } break;
	case ECRPhase::BossRush:
		{
			const FVector NL = Boss->GetActorLocation() + Data.CRDirection * CR_RushSpeed * DeltaTime;
			const float TD = FVector::Dist2D(Data.CRStartLocation, Data.CRTargetLocation);
			if (FVector::Dist2D(Data.CRStartLocation, NL) >= TD) { Boss->SetActorLocation(Data.CRTargetLocation); Data.CRPhase = ECRPhase::Recovery; Data.PhaseTimer = 0.f; }
			else { Boss->SetActorLocation(NL); if (!Data.bBossRushDamageApplied) { ApplyDamageInRadius(Boss, Boss->GetActorLocation(), CR_RushWidth, CR_Damage, CR_KnockbackForce, Data.CRDirection); TArray<FOverlapResult> O; FCollisionQueryParams Q; Q.AddIgnoredActor(Boss); Boss->GetWorld()->OverlapMultiByChannel(O,Boss->GetActorLocation(),FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(CR_RushWidth),Q); for(auto&R:O) if(Cast<ACharacter>(R.GetActor())&&R.GetActor()!=Boss){Data.bBossRushDamageApplied=true;break;} } }
		} break;
	case ECRPhase::Recovery:
		if (Data.PhaseTimer >= CR_RecoveryDuration) return EStateTreeRunStatus::Succeeded; break;
	default: return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}
 

// 공중 전류

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterAerialElectric(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player) return EStateTreeRunStatus::Failed;
	Data.AEPhase = EAEPhase::Vanishing;
	Data.AEOriginalLocation = Boss->GetActorLocation();
	Data.AEGroundTarget = Player->GetActorLocation();
	Data.AEHoverLocation = FVector(Data.AEGroundTarget.X, Data.AEGroundTarget.Y, Data.AEGroundTarget.Z + AE_HoverHeight);
	Boss->SetActorHiddenInGame(true); Boss->SetActorEnableCollision(false);
	UE_LOG(LogTemp, Log, TEXT("[AerialElectric] 시작"));
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossAttackExecutor::TickAerialElectric(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
	switch (Data.AEPhase)
	{
	case EAEPhase::Vanishing:
		if (Data.PhaseTimer >= AE_VanishDuration)
		{ Boss->SetActorLocation(Data.AEHoverLocation); Boss->SetActorHiddenInGame(false); Data.AEPhase = EAEPhase::Hovering; Data.PhaseTimer = 0.f; } break;
	case EAEPhase::Hovering:
		if (Data.PhaseTimer >= 0.2f) { Data.AEPhase = EAEPhase::Charging; Data.PhaseTimer = 0.f; } break;
	case EAEPhase::Charging:
#if ENABLE_DRAW_DEBUG
		DrawDebugCircle(Boss->GetWorld(), Data.AEGroundTarget+FVector(0,0,5), AE_DamageRadius, 32, FColor::Red, false, DeltaTime*2.f, 0, 3.f, FVector(1,0,0), FVector(0,1,0), false);
#endif
		if (Data.PhaseTimer >= AE_ChargeDuration) { Data.AEPhase = EAEPhase::Discharge; Data.PhaseTimer = 0.f; Data.bDamageApplied = false; } break;
	case EAEPhase::Discharge:
		if (!Data.bDamageApplied) { ApplyDamageInRadius(Boss, Data.AEGroundTarget, AE_DamageRadius, AE_Damage, AE_KnockbackForce, FVector::UpVector); Data.bDamageApplied = true; }
#if ENABLE_DRAW_DEBUG
		DrawDebugCircle(Boss->GetWorld(), Data.AEGroundTarget+FVector(0,0,5), AE_DamageRadius, 32, FColor::Yellow, false, DeltaTime*2.f, 0, 5.f, FVector(1,0,0), FVector(0,1,0), false);
#endif
		if (Data.PhaseTimer >= AE_DischargeDuration) { Data.AEPhase = EAEPhase::Landing; Data.PhaseTimer = 0.f; } break;
	case EAEPhase::Landing:
		{ float A = FMath::Clamp(Data.PhaseTimer/AE_LandingDuration, 0.f, 1.f);
		  Boss->SetActorLocation(FMath::Lerp(Data.AEHoverLocation, FVector(Data.AEGroundTarget.X,Data.AEGroundTarget.Y,Data.AEOriginalLocation.Z), A));
		  if (A >= 1.f) { Boss->SetActorEnableCollision(true); return EStateTreeRunStatus::Succeeded; }
		} break;
	default: return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}
 

// 얼음가시

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterIceSpikes(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player) return EStateTreeRunStatus::Failed;
	Data.ISPhase = EISPhase::WindUp; Data.ISSpawnedCount = 0; Data.ISSpawnTimer = 0.f;
	Data.ISOrigin = Boss->GetActorLocation();
	const FVector ToP = Player->GetActorLocation() - Data.ISOrigin;
	Data.ISDirection = FVector(ToP.X, ToP.Y, 0.f).GetSafeNormal();
	Boss->SetActorRotation(Data.ISDirection.Rotation());
	UE_LOG(LogTemp, Log, TEXT("[IceSpikes] 시작"));
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossAttackExecutor::TickIceSpikes(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
	switch (Data.ISPhase)
	{
	case EISPhase::WindUp:
		if (Data.PhaseTimer >= IS_WindUpDuration) { Data.ISPhase = EISPhase::Warning; Data.PhaseTimer = 0.f; } break;
	case EISPhase::Warning:
		{
			const FVector RV = FVector(-Data.ISDirection.Y, Data.ISDirection.X, 0.f);
			const FVector AC = Data.ISOrigin + Data.ISDirection * (IS_AreaStartOffset + IS_AreaLength*0.5f);
			const FVector FH = Data.ISDirection*(IS_AreaLength*0.5f), RH = RV*(IS_AreaWidth*0.5f);
			const FVector G(0,0,5);
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(Boss->GetWorld(), AC-FH-RH+G, AC-FH+RH+G, FColor::Red, false, DeltaTime*2.f, 0, 4.f);
			DrawDebugLine(Boss->GetWorld(), AC-FH+RH+G, AC+FH+RH+G, FColor::Red, false, DeltaTime*2.f, 0, 4.f);
			DrawDebugLine(Boss->GetWorld(), AC+FH+RH+G, AC+FH-RH+G, FColor::Red, false, DeltaTime*2.f, 0, 4.f);
			DrawDebugLine(Boss->GetWorld(), AC+FH-RH+G, AC-FH-RH+G, FColor::Red, false, DeltaTime*2.f, 0, 4.f);
#endif
			if (Data.PhaseTimer >= IS_WarningDuration) { Data.ISPhase = EISPhase::Spawning; Data.PhaseTimer = 0.f; }
		} break;
	case EISPhase::Spawning:
		{
			const FVector RV = FVector(-Data.ISDirection.Y, Data.ISDirection.X, 0.f);
			for (int32 i = 0; i < IS_SpikeCount; i++)
			{
				const FVector SL = Data.ISOrigin + Data.ISDirection*FMath::RandRange(IS_AreaStartOffset, IS_AreaStartOffset+IS_AreaLength)
					+ RV*FMath::RandRange(-IS_AreaWidth*0.5f, IS_AreaWidth*0.5f);
				ApplyDamageInRadius(Boss, SL, IS_SpikeRadius, IS_Damage, IS_KnockbackForce, FVector::UpVector);
#if ENABLE_DRAW_DEBUG
				DrawDebugCylinder(Boss->GetWorld(), SL-FVector(0,0,10), SL+FVector(0,0,250), IS_SpikeRadius*0.25f, 6, FColor::Cyan, false, IS_HoldDuration+0.5f, 0, 3.f);
#endif
			}
			Data.ISPhase = EISPhase::Holding; Data.PhaseTimer = 0.f;
			UE_LOG(LogTemp, Log, TEXT("[IceSpikes] 가시 %d개 생성!"), IS_SpikeCount);
		} break;
	case EISPhase::Holding:
		if (Data.PhaseTimer >= IS_HoldDuration) { Data.ISPhase = EISPhase::Fading; Data.PhaseTimer = 0.f; } break;
	case EISPhase::Fading:
		if (Data.PhaseTimer >= 0.3f) return EStateTreeRunStatus::Succeeded; break;
	default: return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}
 


//  전류구 (ElectricOrbs) — Phase2+
//  보스 주변에 5개 전류구 생성 → 플레이어를 향해 순차 발사

EStateTreeRunStatus FSTTask_BossAttackExecutor::EnterElectricOrbs(
	FInstanceDataType& Data, ACharacter* Boss) const
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
	if (!Player) return EStateTreeRunStatus::Failed;
 
	Data.OOPhase = EOOPhase::Charging;
	Data.OOLaunchedCount = 0;
	Data.OOLaunchTimer = 0.f;
	Data.OOOrbPositions.Empty();
	Data.OOOrbDirections.Empty();
	Data.OOOrbHit.Empty();
 
	// 보스 앞에서 가로 일렬 배치
	const FVector BossLoc = Boss->GetActorLocation();
	const FVector FwdDir = Boss->GetActorForwardVector();
	const FVector RightDir = FVector(-FwdDir.Y, FwdDir.X, 0.f);
 
	// 보스 앞 200cm 지점, 허리 높이 80cm
	const float ForwardOffset = 200.f;
	const float HeightOffset = 80.f;
	const FVector LineCenter = BossLoc + FwdDir * ForwardOffset + FVector(0, 0, HeightOffset);
 
	for (int32 i = 0; i < OO_OrbCount; i++)
	{
		// 가로로 균등 배치 (-2, -1, 0, 1, 2 식으로)
		const float HalfCount = (OO_OrbCount - 1) * 0.5f;
		const float Offset = (i - HalfCount) * OO_OrbitRadius;
		const FVector OrbPos = LineCenter + RightDir * Offset;
 
		Data.OOOrbPositions.Add(OrbPos);
		Data.OOOrbDirections.Add(FVector::ZeroVector);
		Data.OOOrbHit.Add(false);
	}
 
	Boss->SetActorRotation((Player->GetActorLocation() - BossLoc).GetSafeNormal2D().Rotation());
 
	UE_LOG(LogTemp, Log, TEXT("[ElectricOrbs] 전류구 %d개 충전 시작"), OO_OrbCount);
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossAttackExecutor::TickElectricOrbs(
	FInstanceDataType& Data, ACharacter* Boss, float DeltaTime) const
{
	Data.PhaseTimer += DeltaTime;
 
	switch (Data.OOPhase)
	{
	case EOOPhase::Charging:
		{
#if ENABLE_DRAW_DEBUG
			for (const FVector& Pos : Data.OOOrbPositions)
				DrawDebugSphere(Boss->GetWorld(), Pos, OO_OrbRadius, 8, FColor::Purple, false, DeltaTime*2.f, 0, 2.f);
#endif
			if (Data.PhaseTimer >= OO_ChargeDuration)
			{
				// 일제 발사 — 모든 전류구 방향 초기화
				const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
				for (int32 i = 0; i < OO_OrbCount; i++)
				{
					if (Player)
					{
						FVector Dir = Player->GetActorLocation() - Data.OOOrbPositions[i];
						Dir.Z = 0.f;
						Data.OOOrbDirections[i] = Dir.GetSafeNormal();
					}
				}
				Data.OOPhase = EOOPhase::Flying;
				Data.PhaseTimer = 0.f;
				UE_LOG(LogTemp, Log, TEXT("[ElectricOrbs] 전류구 %d개 일제 발사!"), OO_OrbCount);
			}
		}
		break;
 
	case EOOPhase::Flying:
		{
			const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Boss->GetWorld(), 0);
			const FVector PlayerLoc = Player ? Player->GetActorLocation() : FVector::ZeroVector;
 
			bool bAllDone = true;
			for (int32 i = 0; i < OO_OrbCount; i++)
			{
				if (Data.OOOrbHit[i]) continue;
				bAllDone = false;
 
				// 플레이어 방향으로 천천히 회전 (추적)
				if (Player)
				{
					FVector DesiredDir = PlayerLoc - Data.OOOrbPositions[i];
					DesiredDir.Z = 0.f;
					DesiredDir = DesiredDir.GetSafeNormal();
 
					// 부드러운 방향 보간
					Data.OOOrbDirections[i] = FMath::VInterpNormalRotationTo(
						Data.OOOrbDirections[i], DesiredDir,
						DeltaTime, OO_TrackingStrength);
				}
 
				// 이동
				Data.OOOrbPositions[i] += Data.OOOrbDirections[i] * OO_OrbSpeed * DeltaTime;
 
				// 최대 생존 시간 체크
				if (Data.PhaseTimer >= OO_MaxLifetime)
				{
					Data.OOOrbHit[i] = true;
					continue;
				}
 
				// 히트 판정
				TArray<FOverlapResult> Overlaps;
				FCollisionQueryParams QP; QP.AddIgnoredActor(Boss);
				Boss->GetWorld()->OverlapMultiByChannel(Overlaps, Data.OOOrbPositions[i],
					FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(OO_OrbRadius), QP);
				for (auto& O : Overlaps)
				{
					AActor* HitActor = O.GetActor();
					if (HitActor && HitActor != Boss && DamageableHelpers::IsDamageable(HitActor))
					{
						DamageableHelpers::ApplyDamage(HitActor, Boss, static_cast<int>(OO_Damage));
						if (ACharacter* HitChar = Cast<ACharacter>(HitActor))
							if (auto* M = HitChar->GetCharacterMovement())
								M->AddImpulse(Data.OOOrbDirections[i] * OO_KnockbackForce, true);
						UE_LOG(LogTemp, Log, TEXT("[ElectricOrbs] %s 히트! %.0f 데미지"),
							*HitActor->GetName(), OO_Damage);
						Data.OOOrbHit[i] = true;
						break;
					}
				}
 
#if ENABLE_DRAW_DEBUG
				if (!Data.OOOrbHit[i])
					DrawDebugSphere(Boss->GetWorld(), Data.OOOrbPositions[i], OO_OrbRadius,
						8, FColor::Purple, false, DeltaTime*2.f, 0, 2.f);
#endif
			}
 
			if (bAllDone)
			{
				UE_LOG(LogTemp, Log, TEXT("[ElectricOrbs] 완료"));
				return EStateTreeRunStatus::Succeeded;
			}
		}
		break;
 
	default: return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
}
 

// 공통 데미지 (IDamageable 인터페이스 사용)

void FSTTask_BossAttackExecutor::ApplyDamageInRadius(
	AActor* BossActor, const FVector& Center, float Radius,
	float Damage, float Knockback, const FVector& KnockbackDir) const
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QP; QP.AddIgnoredActor(BossActor);
	BossActor->GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Radius), QP);
	for (auto& O : Overlaps)
	{
		AActor* HitActor = O.GetActor();
		if (!HitActor || HitActor == BossActor) continue;
 
		// IDamageable 인터페이스로 데미지 적용
		if (DamageableHelpers::ApplyDamage(HitActor, BossActor, static_cast<int>(Damage)))
		{
			UE_LOG(LogTemp, Log, TEXT("[Attack] %s에게 %.0f 데미지!"), *HitActor->GetName(), Damage);
		}
 
		// 넉백 (ACharacter만)
		if (ACharacter* HitChar = Cast<ACharacter>(HitActor))
		{
			if (auto* M = HitChar->GetCharacterMovement())
				M->AddImpulse((HitChar->GetActorLocation() - BossActor->GetActorLocation()).GetSafeNormal() * Knockback, true);
		}
	}
}
