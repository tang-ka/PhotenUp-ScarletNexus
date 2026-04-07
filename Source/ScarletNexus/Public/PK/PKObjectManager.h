// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKObjectManager.generated.h"

class APKObject;
// Pool에 등록할 오브젝트 종류별 설정
USTRUCT(BlueprintType)
struct FPKPoolEntry
{
	GENERATED_BODY()
	
	// 스폰할 PKObject 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pool")
	TSubclassOf<APKObject> PKObjectClass;
	
	// 동시에 월드에 존재할 최대 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pool", meta=(ClampMin = "1"))
	int32 PoolSize = 3;
	
	// 사용(던지기 등) 후 리스폰까지 딜레이 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pool", meta=(ClampMin = "0.0"))
	float RespawnDelay = 5.f;
	
	// 스폰 기준 위치 (매니저 기준 로컬 오프셋)
	// 비어있으면 매니저 위치 주변 랜덤 배치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pool")
	TArray<FVector> SpawnOffsetList;
	
	// SpawnOffsetList가 비어있을 때 랜덤 배치 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pool", meta=(ClampMin = "0.0"))
	float  RandomSpawnRadius = 300.f;
};

// pool 내부에서 개별 슬롯 상태 추적
USTRUCT()
struct FPKPoolSlot
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<APKObject> Object = nullptr;
	
	// 현재 활성 상태 체크
	bool bActive = false;
	
	// 리스폰 대기중 체크
	bool bWaitingRespawn = false;
	
	// 이 슬롯의 스폰 위치 (월드 좌표)
	FVector SpawnLocation = FVector::ZeroVector;
	
	// 이 슬롯의 스폰 회전
	FRotator SpawnRotation = FRotator::ZeroRotator;
	
	// 소속 엔트리 인덱스
	int32 EntryIndex = INDEX_NONE;
	
	// 슬롯 인덱스 (엔트리 내)
	int32 SlotIndex = INDEX_NONE;
};

UCLASS()
class SCARLETNEXUS_API APKObjectManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APKObjectManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// ** 에디터 설정
	// pool에 등록할 오브젝트 종류 리스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PK Pool")
	TArray<FPKPoolEntry> PoolEntries;
	
	// ** 외부 API
	// PKObject가 사용되었을 때 호출 -> 즉시 비활성 + 리스폰 예약
	UFUNCTION(BlueprintCallable, Category="PK Pool")
	void ReturnObject(APKObject* Object);
	
	// 던진 후에 날아가는 연출이 끝날 때까지 대기 후 비활성 + 리스폰 예약
	// HideDelay : 던진 후 몇 초 후에 사라질지 지정 (날아가는 시간)
	UFUNCTION(BlueprintCallable, Category="PK Pool")
	void ReturnObjectDelayed(APKObject* Object, float HideDelay = 2.f);
	
	// 특정 클래스의 활성 오브젝트 중 위치가 최근접한 오브젝트 반환
	UFUNCTION(BlueprintCallable, Category="PK Pool")
	APKObject* GetNearestActiveObject(FVector Origin, TSubclassOf<APKObject> FilterClass = nullptr) const;
	
	// 전역 싱글톤 접근
	UFUNCTION(BlueprintCallable, Category="PK Pool", meta = (WorldContext = "WorldContextObject"))
	static APKObjectManager* Get(const UObject* WorldContextObject);
	
private:
	// 전체 슬롯 (전체 엔트리의 슬롯을 플랫하게 저장)
	UPROPERTY()
	TArray<FPKPoolSlot> AllSlots;
	
	// 오브젝트 -> 슬롯 인덱스 빠른 역참조
	UPROPERTY()
	TMap<TObjectPtr<APKObject>, int32> ObjectToSlotIndex;
	
	// 리스폰 타이머 정보
	struct FRespawnTimer
	{
		int32 SlotIndex;
		float RemainingTime;
	};
	TArray<FRespawnTimer> RespawnTimers;
	
	// 비활성화 지연 타이머 (던진 후 날아가는 동안 대기)
	struct FDeactivateTimer
	{
		int32 SlotIndex;
		float RemainingTime;
	};
	TArray<FDeactivateTimer> DeactivateTimers;
	
	// 초기 스폰
	void InitPool();
	
	// 슬롯 하나 스폰
	void SpawnSlot(FPKPoolSlot& Slot);
	
	// 슬롯 비활성화 
	void DeactivateSlot(FPKPoolSlot& Slot);
	
	// 슬롯 재활성화
	void ReactivateSlot(FPKPoolSlot& Slot);
	
	// 스폰 위치 계산
	FVector CalcSpawnLocation(const FPKPoolEntry& Entry, int32 SlotIdx) const;
};
