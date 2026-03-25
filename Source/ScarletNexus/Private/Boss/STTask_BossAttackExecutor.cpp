// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossAttackExecutor.h"
#include "Boss/BossCloneActor.h"
#include "Boss/BossCharacterBase.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
 

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
 
	Data.ActiveAttack = SelectAttack(Boss);
 
	switch (Data.ActiveAttack)
	{
	case EActiveAttackType::TeleportKick:    return EnterTeleportKick(Data, Boss);
	case EActiveAttackType::CloneRush:       return EnterCloneRush(Data, Boss);
	case EActiveAttackType::AerialElectric:  return EnterAerialElectric(Data, Boss);
	case EActiveAttackType::IceSpikes:       return EnterIceSpikes(Data, Boss);
	case EActiveAttackType::ElectricOrbs:    return EnterElectricOrbs(Data, Boss);
	// case EActiveAttackType::TelekinesisThrow:return EnterTelekinesisThrow(Data, Boss);
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
 
	switch (Data.ActiveAttack)
	{
	case EActiveAttackType::TeleportKick:    return TickTeleportKick(Data, Boss, DeltaTime);
	case EActiveAttackType::CloneRush:       return TickCloneRush(Data, Boss, DeltaTime);
	case EActiveAttackType::AerialElectric:  return TickAerialElectric(Data, Boss, DeltaTime);
	case EActiveAttackType::IceSpikes:       return TickIceSpikes(Data, Boss, DeltaTime);
	case EActiveAttackType::ElectricOrbs:    return TickElectricOrbs(Data, Boss, DeltaTime);
	// case EActiveAttackType::TelekinesisThrow:return TickTelekinesisThrow(Data, Boss, DeltaTime);
	default: return EStateTreeRunStatus::Failed;
	}
}
 

// ExitState

void FSTTask_BossAttackExecutor::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	if (AActor* Boss = Data.ContextActor)
	{
		Boss->SetActorHiddenInGame(false);
		Boss->SetActorEnableCollision(true);
	}
	if (Data.LeftClone && !Data.LeftClone->IsActorBeingDestroyed()) Data.LeftClone->Destroy();
	if (Data.RightClone && !Data.RightClone->IsActorBeingDestroyed()) Data.RightClone->Destroy();
	Data.LeftClone = nullptr;
	Data.RightClone = nullptr;
}
 

// 공격 선택 (페이즈별)

EActiveAttackType FSTTask_BossAttackExecutor::SelectAttack(const ACharacter* Boss) const
{
	TArray<EActiveAttackType> Pool;
	// Phase1 기본
	Pool.Add(EActiveAttackType::TeleportKick);
	Pool.Add(EActiveAttackType::CloneRush);
	Pool.Add(EActiveAttackType::AerialElectric);
	Pool.Add(EActiveAttackType::IceSpikes);
 
	// 페이즈별 추가
	if (const ABossCharacterBase* BossChar = Cast<ABossCharacterBase>(Boss))
	{
		const EBossPhase Phase = BossChar->GetCurrentPhase();
 
		if (Phase >= EBossPhase::Phase2)
		{
			Pool.Add(EActiveAttackType::ElectricOrbs);
		}
		// TelekinesisThrow는 나중에 맵 액터 연동해서 형들이랑 추가하는걸로
	}
 
	const int32 Roll = FMath::RandRange(0, Pool.Num() - 1);
	const EActiveAttackType Selected = Pool[Roll];
 
	const TCHAR* Names[] = {
		TEXT("None"), TEXT("텔레포트 킥"), TEXT("분신 돌진"),
		TEXT("공중 전류"), TEXT("얼음가시"), TEXT("전류구"), TEXT("염동력 투척") };
	UE_LOG(LogTemp, Log, TEXT("[AttackExecutor] 선택: %s (풀: %d개)"),
		Names[static_cast<int32>(Selected)], Pool.Num());
 
	return Selected;
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
	const FVector LL = Data.CRStartLocation - RV * CR_CloneSpacing;
	if (ABossCloneActor* LC = Boss->GetWorld()->SpawnActor<ABossCloneActor>(ABossCloneActor::StaticClass(), LL, FRotator::ZeroRotator, SP))
	{
		FVector LD = FVector(Player->GetActorLocation().X-LL.X, Player->GetActorLocation().Y-LL.Y, 0.f).GetSafeNormal();
		float LDist = FVector::Dist2D(LL, Player->GetActorLocation()) + 200.f;
		LC->InitRush(LD, CR_RushSpeed, LDist, CR_Damage, CR_RushWidth, CR_KnockbackForce);
		if (auto* BM = Boss->GetMesh()) if (auto* CM = LC->FindComponentByClass<USkeletalMeshComponent>()) CM->SetSkeletalMesh(BM->GetSkeletalMeshAsset());
		Data.LeftClone = LC;
	}
	const FVector RL = Data.CRStartLocation + RV * CR_CloneSpacing;
	if (ABossCloneActor* RC = Boss->GetWorld()->SpawnActor<ABossCloneActor>(ABossCloneActor::StaticClass(), RL, FRotator::ZeroRotator, SP))
	{
		FVector RD = FVector(Player->GetActorLocation().X-RL.X, Player->GetActorLocation().Y-RL.Y, 0.f).GetSafeNormal();
		float RDist = FVector::Dist2D(RL, Player->GetActorLocation()) + 200.f;
		RC->InitRush(RD, CR_RushSpeed, RDist, CR_Damage, CR_RushWidth, CR_KnockbackForce);
		if (auto* BM = Boss->GetMesh()) if (auto* CM = RC->FindComponentByClass<USkeletalMeshComponent>()) CM->SetSkeletalMesh(BM->GetSkeletalMeshAsset());
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
 
	// 보스 주변에 전류구 원형 배치
	const FVector BossLoc = Boss->GetActorLocation();
	for (int32 i = 0; i < OO_OrbCount; i++)
	{
		const float Angle = (2.f * PI / OO_OrbCount) * i;
		const FVector OrbPos = BossLoc + FVector(
			FMath::Cos(Angle) * OO_OrbitRadius,
			FMath::Sin(Angle) * OO_OrbitRadius,
			150.f); // 보스 허리 높이
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
					ACharacter* Hit = Cast<ACharacter>(O.GetActor());
					if (Hit && Hit != Boss)
					{
						FDamageEvent DE;
						Hit->TakeDamage(OO_Damage, DE, nullptr, Boss);
						if (auto* M = Hit->GetCharacterMovement())
							M->AddImpulse(Data.OOOrbDirections[i] * OO_KnockbackForce, true);
						UE_LOG(LogTemp, Log, TEXT("[ElectricOrbs] %s 히트! %.0f 데미지"),
							*Hit->GetName(), OO_Damage);
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

 

// 공통 데미지

void FSTTask_BossAttackExecutor::ApplyDamageInRadius(
	AActor* BossActor, const FVector& Center, float Radius,
	float Damage, float Knockback, const FVector& KnockbackDir) const
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QP; QP.AddIgnoredActor(BossActor);
	BossActor->GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Radius), QP);
	for (auto& O : Overlaps)
	{
		ACharacter* Hit = Cast<ACharacter>(O.GetActor());
		if (!Hit || Hit == BossActor) continue;
		FDamageEvent DE; Hit->TakeDamage(Damage, DE, nullptr, BossActor);
		UE_LOG(LogTemp, Log, TEXT("[Attack] %s에게 %.0f 데미지!"), *Hit->GetName(), Damage);
		if (auto* M = Hit->GetCharacterMovement())
			M->AddImpulse((Hit->GetActorLocation()-BossActor->GetActorLocation()).GetSafeNormal()*Knockback, true);
	}
}
