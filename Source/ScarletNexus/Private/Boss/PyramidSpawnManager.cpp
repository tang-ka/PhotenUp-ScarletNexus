// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/PyramidSpawnManager.h"

#include "ScarletNexus.h"
#include "Boss/FloatingPyramid.h"


// Sets default values
APyramidSpawnManager::APyramidSpawnManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APyramidSpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (PyramidClass)
	{
		SpawnAllPyramids();
	}
	else
	{
		PRINTLOG_GT(TEXT("PyramidClass가 지정되지 않았다!"));
	}
}

// Called every frame
void APyramidSpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 파괴된 피라미드 정리
	SpawnedPyramids.RemoveAll([](const TWeakObjectPtr<AFloatingPyramid>& Ptr)
	{
		return !Ptr.IsValid();
	});
	
	// 부족한 수만큼 리스폰 대기
	if (SpawnedPyramids.Num() < MaxCount)
	{
		RespawnTimer += DeltaTime;
		
		if (RespawnTimer >= RespawnDelay)
		{
			SpawnOnePyramid();
			RespawnTimer = 0.f;
		}
	}
	else
	{
		RespawnTimer = 0.f;
	}
}

void APyramidSpawnManager::SpawnAllPyramids()
{
	const int32 toSpawn = MaxCount - SpawnedPyramids.Num();
	for (int32 i = 0; i < toSpawn; ++i)
	{
		SpawnOnePyramid();
	}
}

void APyramidSpawnManager::DestroyAllPyramids()
{
	for (auto& ptr : SpawnedPyramids)
	{
		if (ptr.IsValid()) ptr->Destroy();
	}
	SpawnedPyramids.Empty();
}

FVector APyramidSpawnManager::GetRandomSpawnLocation() const
{
	// 매니저 위치 중심으로 원형 범위 내 랜덤
	const float angle = FMath::FRandRange(0.f, 2.f * PI);
	const float dist = FMath::FRandRange(SpawnRadius * 0.3f, SpawnRadius);
	const float heightOffset = FMath::FRandRange(MinHeight, MaxHeight);
	
	const FVector origin = GetActorLocation();
	return FVector(
		origin.X + FMath::Cos(angle) * dist,
		origin.Y + FMath::Sin(angle) * dist,
		origin.Z + heightOffset);
}

void APyramidSpawnManager::SpawnOnePyramid()
{
	if (!PyramidClass) return;
	
	const FVector loc = GetRandomSpawnLocation();
	const FRotator rot = FRotator(0.f, FMath::FRandRange(0.f, 2.0f * PI), 0.f);
	
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AFloatingPyramid* pyramid = GetWorld()->SpawnActor<AFloatingPyramid>(PyramidClass, loc, rot, params);
	if (pyramid)
	{
		// 스케일 랜덤
		const float randScale = FMath::FRandRange(MinScale, MaxScale);
		pyramid->SetActorScale3D(FVector(randScale));
		
		SpawnedPyramids.Add(pyramid);
		PRINTLOG_GT(TEXT("Spawned at %s (Total: %d)"), *loc.ToString(), SpawnedPyramids.Num());
	}
}
