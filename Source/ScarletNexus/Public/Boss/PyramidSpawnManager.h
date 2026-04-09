// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PyramidSpawnManager.generated.h"

/**
 * 보스전 구역에 배치 → 범위 내에 떠다니는 사각뿔들을 자동 스폰.
 * 파괴된 피라미드는 일정 시간 후 리스폰.
 */
UCLASS()
class SCARLETNEXUS_API APyramidSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APyramidSpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// ── 스폰 설정 ──
	// 스폰할 FloatingPyramid 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<class AFloatingPyramid> PyramidClass;
 
	// 한 번에 유지할 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 MaxCount = 8;
 
	// 스폰 반경 (매니저 위치 기준, cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float SpawnRadius = 800.f;
 
	// 최소 높이 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float MinHeight = 150.f;
 
	// 최대 높이 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float MaxHeight = 400.f;
 
	// 파괴 후 리스폰 대기 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float RespawnDelay = 5.f;
 
	// 수동 스폰/전체 제거
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnAllPyramids();
 
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void DestroyAllPyramids();
	
private:
	FVector GetRandomSpawnLocation() const;
	void SpawnOnePyramid();
 
	UPROPERTY()
	TArray<TWeakObjectPtr<AFloatingPyramid>> SpawnedPyramids;
 
	// 리스폰 타이머
	float RespawnTimer = 0.f;
};
