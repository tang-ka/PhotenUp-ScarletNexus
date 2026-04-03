// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyManager.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Chaos/ChaosPerfTest.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"


AEnemyManager::AEnemyManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	LoadWaveRows();
	InitPool();
	
	if (bAutoStartFirstWave && WaveRows.Num() > 0)
	{
		StartWave(0);
	}
}

void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyManager::StartNextWave()
{
	if (CurrentWaveIndex < WaveRows.Num())
	{
		StartWave(CurrentWaveIndex);
	}
}

void AEnemyManager::StartWave(int32 WaveIndex)
{
	if (!WaveRows.IsValidIndex(WaveIndex)) return;
	
	CurrentWaveIndex = WaveIndex;
	const FEnemyWave* wave = WaveRows[WaveIndex];
	
	if (wave->DelayBeforeWave > 0.f)
	{
		// 딜레이 후 스폰
		GetWorldTimerManager().SetTimer(
			WaveDelayTimerHandle,
			[this, WaveIndex]()
			{
				if (WaveRows.IsValidIndex(WaveIndex))
				{
					OnWaveStarted.Broadcast(WaveIndex);
					SpawnWaveEnemies(*WaveRows[WaveIndex]);
				}
			},
			wave->DelayBeforeWave,
			false);
	}
	else
	{
		OnWaveStarted.Broadcast(WaveIndex);
		SpawnWaveEnemies(*wave);
	}
}

void AEnemyManager::OnEnemyDied(AEnemyBase* Enemy)
{
	if (!Enemy) return;
	
	AliveEnemies.Remove(Enemy);
	
	// 풀로 반환 (딜레이 후 DestroyDelay와 맞춤
	FTimerHandle returnHandle;
	GetWorldTimerManager().SetTimer(
		returnHandle,
		[this, Enemy]()
		{
			ReturnToPool(Enemy);
		},
		Enemy->DestroyDelay,
		false);
	
	// 현재 웨이브 클리어 체크
	if (AliveEnemies.Num() == 0)
	{
		OnWaveCleared.Broadcast(CurrentWaveIndex);
		CurrentWaveIndex++;
		
		if (CurrentWaveIndex >= WaveRows.Num())
		{
			// 전체 웨이브 클리어
			OnAllEnemiesDefeated.Broadcast();
		}
		else
		{
			// 다음 웨이브
			StartNextWave();
		}
	}
}

void AEnemyManager::LoadWaveRows()
{
	WaveRows.Empty();
	
	if (!WaveDataTable) return;
	
	TArray<FEnemyWave*> rows;
	WaveDataTable->GetAllRows<FEnemyWave>(TEXT("EnemyManager"), rows);
	
	// Row 이름 순서대로 들어오므로 그대로 사용
	// (DataTable에서 Row 이름을 Wave_00, Wave_01... 으로 지으면 순서 보장)
	WaveRows = rows;
}

void AEnemyManager::InitPool()
{
	// 웨이브에 등장하는 모든 클래스에 대해 풀 미리 생성
	TSet<TSubclassOf<AEnemyBase>> UniqueClasses;
	
	for (const FEnemyWave* wave : WaveRows)
	{
		for (const FEnemyWaveEntry& entry : wave->Entries)
		{
			if (entry.EnemyClass)
			{
				UniqueClasses.Add(entry.EnemyClass);
			}
		}
	}
	
	for (const TSubclassOf<AEnemyBase>& enemyClass : UniqueClasses)
	{
		TArray<AEnemyBase*> pool = EnemyPool.FindOrAdd(enemyClass);
		
		for (int32 i = 0; i < PoolSizePerClass; ++i)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			AEnemyBase* enemy = GetWorld()->SpawnActor<AEnemyBase>
			(
				enemyClass,
				FVector(0.0f, 0.0f, -5000.0f), // 스크린 밖에 생성
				FRotator::ZeroRotator,
				spawnParams
				);
			
			if (enemy)
			{
				DeactivateEnemy(enemy);
				pool.Add(enemy);
			}
		}
	}
}

AEnemyBase* AEnemyManager::GetFromPool(TSubclassOf<AEnemyBase> EnemyClass)
{
	TArray<AEnemyBase*>* pool = EnemyPool.Find(EnemyClass);
	if (pool && pool->Num() > 0)
	{
		AEnemyBase* enemy = pool->Pop();
		return enemy;
	}
	
	// 풀이 비었으면 새로 스폰
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AEnemyBase* enemy = GetWorld()->SpawnActor<AEnemyBase>(
		EnemyClass,
		FVector(0.0f, 0.0f, -5000.0f),
		FRotator::ZeroRotator,
		spawnParams
		);
	
	return enemy;
}

void AEnemyManager::ReturnToPool(AEnemyBase* Enemy)
{
	if (!Enemy) return;
	
	DeactivateEnemy(Enemy);
	
	TSubclassOf<AEnemyBase> enemyClass = Enemy->GetClass();
	TArray<AEnemyBase*>& pool = EnemyPool.FindOrAdd(enemyClass);
	pool.Add(Enemy);
}

void AEnemyManager::ActivateEnemy(AEnemyBase* Enemy, const FVector& Location, const FRotator& Rotaion)
{
	if (!Enemy) return;
	
	Enemy->SetActorLocation(Location);
	Enemy->SetActorRotation(Rotaion);
	Enemy->SetActorHiddenInGame(false);
	Enemy->SetActorEnableCollision(true);
	Enemy->SetActorTickEnabled(true);
	
	// HP 초기화
	Enemy->CurrHP = Enemy->MaxHP;
	Enemy->bIsDie = false;
	Enemy->bIsStunned = false;
	Enemy->OwningManager = this;
	
	// 체력바 표시
	if (Enemy->HealthBarComp)
	{
		Enemy->HealthBarComp->SetVisibility(true);
	}
	
	// 캡슐 충돌 복원
	if (UCapsuleComponent* capsule = Enemy->GetCapsuleComponent())
	{
		capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	
	// 이동 복원
	if (UCharacterMovementComponent* moveComp = Enemy->GetCharacterMovement())
	{
		moveComp->SetMovementMode(MOVE_Walking);
	}
	
	// AI 재시작 - Controller 재빙의
	Enemy->SpawnDefaultController();
}

void AEnemyManager::DeactivateEnemy(AEnemyBase* Enemy)
{
	if (!Enemy) return;
	
	// AI 정지
	if (AController* controller = Enemy->GetController())
	{
		controller->UnPossess();
	}
	
	// Ragdoll 리셋
	if (USkeletalMeshComponent* meshComp = Enemy->GetMesh())
	{
		meshComp->SetSimulatePhysics(false);
		meshComp->SetAllBodiesSimulatePhysics(false);
		meshComp->SetCollisionProfileName(TEXT("CharacterMesh"));
		meshComp->AttachToComponent(Enemy->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	
	// 체력바 숨기기
	if (Enemy->HealthBarComp)
	{
		Enemy->HealthBarComp->SetVisibility(false);
	}
	
	Enemy->SetActorHiddenInGame(true);
	Enemy->SetActorEnableCollision(false);
	Enemy->SetActorTickEnabled(false);
	Enemy->SetActorLocation(FVector(0.0f, 0.0f, -5000.0f));
}

FTransform AEnemyManager::GetRandomSpawnTransform() const
{
	if (SpawnPoints.Num() == 0)
	{
		// 스폰 포인트 없으면 매니저 위치 사용
		return GetActorTransform();
	}
	
	const int32 idx = FMath::RandRange(0, SpawnPoints.Num() - 1);
	if (SpawnPoints[idx])
	{
		return SpawnPoints[idx]->GetActorTransform();
	}
	
	return GetActorTransform();
}

void AEnemyManager::SpawnWaveEnemies(const FEnemyWave& Wave)
{
	for (const FEnemyWaveEntry& entry : Wave.Entries)
	{
		if (!entry.EnemyClass) continue;
		
		for (int32 i = 0; i < entry.Count; ++i)
		{
			AEnemyBase* enemy = GetFromPool(entry.EnemyClass);
			if (!enemy) continue;
			
			const FTransform spawnTransform = GetRandomSpawnTransform();
			ActivateEnemy(enemy, spawnTransform.GetLocation(), spawnTransform.GetRotation().Rotator());
			
			AliveEnemies.Add(enemy);
		}
	}
}
