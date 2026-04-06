// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

class AEnemyBase;
// ** 웨이브 정의
USTRUCT(BlueprintType)
struct FEnemyWaveEntry
{
	GENERATED_BODY()
	
	// 스폰할 적 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> EnemyClass;
	
	// 이 엔트리에서 스폰할 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 3;
};

USTRUCT(BlueprintType)
struct FEnemyWave : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyWaveEntry> Entries;
	
	// 이 웨이브 시작 전 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBeforeWave = 2.f;
};

// ** 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEnemiesDefeated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCleared, int32, WaveIndex);

UCLASS()
class SCARLETNEXUS_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemyManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	// ** 웨이브 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	TObjectPtr<UDataTable> WaveDataTable;
	
	// 스폰 포인트 (월드에 배치된 Actor들)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	TArray<TObjectPtr<AActor>> SpawnPoints;
	
	// 자동으로 첫 웨이브 시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	bool bAutoStartFirstWave = true;
	
	// ** 스폰 설정
	// Manager 위치 기준 스폰 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	float SpawnRadius = 800.f;
	
	// NavMesh 위에 스폰 시도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	bool bSnapToNavMesh = true;
	
	// ** 풀링 설정
	// 클래스별 풀 초기 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pool")
	int32 PoolSizePerClass = 10;
	
	// ** 웨이브 제어
	// 다음 웨이브 시작
	UFUNCTION(BlueprintCallable, Category="Wave")
	void StartNextWave();
	
	// 특정 웨이브 시작
	UFUNCTION(BlueprintCallable, Category="Wave")
	void StartWave(int32 WaveIndex);
	
	// 현재 웨이브 인덱스
	UFUNCTION(BlueprintCallable, Category="Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }
	
	// 전체 웨이브 완료 되었는지 체크
	UFUNCTION(BlueprintCallable, Category="Wave")
	bool IsAllWavesCleared() const { return CurrentWaveIndex >= WaveRows.Num(); }
	
	// ** 활성 적 추적
	UFUNCTION(BlueprintCallable, Category="Tracking")
	int32 GetAliveEnemyCount() const { return AliveEnemies.Num(); }
	
	UFUNCTION(BlueprintCallable, Category="Tracking")
	const TArray<AEnemyBase*>& GetAliveEnemies() const { return AliveEnemies; }
	
	// 적 사망 시 호출 (EnemyBase에서 콜)
	UFUNCTION(BlueprintCallable, Category="Tracking")
	void OnEnemyDied(AEnemyBase* Enemy);
	
	// ** 이벤트
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnAllEnemiesDefeated OnAllEnemiesDefeated;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnWaveStarted OnWaveStarted;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnWaveCleared OnWaveCleared;
	
	// ** Combat
	// 최대 동시 공격 가능 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	int32 MaxAttackers = 1;
	
	// 공격 토큰 요청 (ture면 공격 가능)
	UFUNCTION(BlueprintCallable, Category="Combat")
	bool RequestAttackToken(AEnemyBase* Enemy);
	
	// 공격 토큰 반환
	UFUNCTION(BlueprintCallable, Category="Combat")
	void ReleaseAttackToken(AEnemyBase* Enemy);
	
private:
	// ** DataTable에서 읽은 Row 포인터들
	TArray<FEnemyWave*> WaveRows;
	void LoadWaveRows();
	
	// ** 풀링
	// 클래스별 비활성 적 풀
	TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>> EnemyPool;
	
	// 풀 초기화
	void InitPool();
	
	// 풀에서 가져오기 (없으면 스폰)
	AEnemyBase* GetFromPool(TSubclassOf<AEnemyBase> EnemyClass);
	
	// 풀로 반환
	void ReturnToPool(AEnemyBase* Enemy);
	
	// 적 활성화
	void ActivateEnemy(AEnemyBase* Enemy, const FVector& Location, const FRotator& Rotaion);
	
	// 적 비활성화 
	void DeactivateEnemy(AEnemyBase* Enemy);
	
	// ** 스폰
	// 스폰 포인트 중 랜덤 위치 반환
	FTransform GetRandomSpawnTransform() const;
	
	// 웨이브의 전체 적 스폰
	void SpawnWaveEnemies(const FEnemyWave& Wave);
	
	// ** 상태
	int32 CurrentWaveIndex = 0;
	
	// 현재 활성 적 목록
	UPROPERTY()
	TArray<AEnemyBase*> AliveEnemies;
	
	// 웨이브 딜레이 타이머
	FTimerHandle WaveDelayTimerHandle;

	// ** 치트
	UFUNCTION(BlueprintCallable, Category="Cheat")
	void Cheat_ToggleAllEnemiesVisible();
	bool bEnemyVisible = true;
	
	// 현재 공격 중인 개체
	UPROPERTY()
	TArray<AEnemyBase*> CurrentAttackers;
};
