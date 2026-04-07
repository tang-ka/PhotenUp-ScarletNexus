// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKObjectManager.h"

#include "Kismet/GameplayStatics.h"
#include "PK/PKObject.h"

// Sets default values
APKObjectManager::APKObjectManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

// ──────────────────────────────────────────────
//  Lifecycle
// ──────────────────────────────────────────────

void APKObjectManager::BeginPlay()
{
	Super::BeginPlay();
	InitPool();
}

void APKObjectManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 비활성화 지연 타이머 처리 (던진 후 날아가는 시간)
	for (int32 i = DeactivateTimers.Num() - 1; i >= 0; --i)
	{
		FDeactivateTimer& timer = DeactivateTimers[i];
		timer.RemainingTime -= DeltaTime;
		
		if (timer.RemainingTime <= 0.f)
		{
			if (AllSlots.IsValidIndex(timer.SlotIndex))
			{
				// 실제 비활성화 + 리스폰 예약
				ReturnObject(AllSlots[timer.SlotIndex].Object);
			}
			DeactivateTimers.RemoveAt(i);
		}
	}
	
	// 리스폰 타이머 처리
	for (int32 i = RespawnTimers.Num() - 1; i >= 0; --i)
	{
		FRespawnTimer& timer = RespawnTimers[i];
		timer.RemainingTime -= DeltaTime;
		
		if (timer.RemainingTime <= 0.f)
		{
			if (AllSlots.IsValidIndex(timer.SlotIndex))
			{
				// 실제 비활성화 + 리스폰 예약
				ReturnObject(AllSlots[timer.SlotIndex].Object);
			}
			DeactivateTimers.RemoveAt(i);
		}
	}
	
	// 리스폰 타이머 처리
	for (int32 i = DeactivateTimers.Num() - 1; i >= 0; --i)
	{
		FRespawnTimer& timer = RespawnTimers[i];
		timer.RemainingTime -= DeltaTime;
		
		if (timer.RemainingTime <= 0.f)
		{
			if (AllSlots.IsValidIndex(timer.SlotIndex))
			{
				ReactivateSlot(AllSlots[timer.SlotIndex]);
			}
			RespawnTimers.RemoveAt(i);
		}
	}
}

// ──────────────────────────────────────────────
//  외부 API
// ──────────────────────────────────────────────
void APKObjectManager::ReturnObject(APKObject* Object)
{
	if (!Object) return;
	
	const int32* slotIdxPtr = ObjectToSlotIndex.Find(Object);
	if (!slotIdxPtr) return;
	
	FPKPoolSlot& slot = AllSlots[*slotIdxPtr];
	if (!slot.bActive) return; // 이미 비활성 상태
	
	DeactivateSlot(slot);
	slot.bWaitingRespawn = true;
	
	// 리스폰 타이머 등록
	if (PoolEntries.IsValidIndex(slot.EntryIndex))
	{
		FRespawnTimer timer;
		timer.SlotIndex = *slotIdxPtr;
		timer.RemainingTime = PoolEntries[slot.EntryIndex].RespawnDelay;
		RespawnTimers.Add(timer);
	}
}

void APKObjectManager::ReturnObjectDelayed(APKObject* Object, float HideDelay)
{
	if (!Object) return;
	
	const int32* slotIdxPtr = ObjectToSlotIndex.Find(Object);
	if (!slotIdxPtr) return;
	
	FPKPoolSlot& slot = AllSlots[*slotIdxPtr];
	if (!slot.bActive) return;
	
	// CanBePickeduped를 false로 설정하여 다른 AI가 집지 못하게 함 (이미 사용중 처리)
	// -> PKObject 블루프린트에서 bUsed 같은 플래그 처리 가능
	FDeactivateTimer timer;
	timer.SlotIndex = *slotIdxPtr;
	timer.RemainingTime = HideDelay;
	DeactivateTimers.Add(timer);
}

APKObject* APKObjectManager::GetNearestActiveObject(FVector Origin, TSubclassOf<APKObject> FilterClass) const
{
	APKObject* best = nullptr;
	float bestDistSq = FLT_MAX;
	
	for (const FPKPoolSlot& slot : AllSlots)
	{
		if (!slot.bActive || !slot.Object) continue;
		
		// 클래스 필터
		if (FilterClass && !slot.Object.IsA(FilterClass)) continue;
		
		const float distSq = FVector::DistSquared(Origin, slot.Object->GetActorLocation());
		if (distSq < bestDistSq)
		{
			bestDistSq = distSq;
			best = slot.Object;
		}
	}
	
	return best;
}

APKObjectManager* APKObjectManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!world) return nullptr;
	
	// 월드에서 첫번째 PKObjectManager 찾기
	AActor* found = UGameplayStatics::GetActorOfClass(world, APKObjectManager::StaticClass());
	return Cast<APKObjectManager>(found);
}

// ──────────────────────────────────────────────
//  초기화
// ──────────────────────────────────────────────
void APKObjectManager::InitPool()
{
	AllSlots.Empty();
	ObjectToSlotIndex.Empty();
	RespawnTimers.Empty();
	
	int32 globalSlotIdx = 0;
	
	for (int32 entryIdx = 0; entryIdx < PoolEntries.Num(); ++entryIdx)
	{
		const FPKPoolEntry& entry = PoolEntries[entryIdx];
		if (!entry.PKObjectClass) continue;
		
		for (int32 slotIdx = 0; slotIdx < entry.PoolSize; ++slotIdx)
		{
			FPKPoolSlot slot;
			slot.EntryIndex = entryIdx;
			slot.SlotIndex = slotIdx;
			slot.SpawnLocation = CalcSpawnLocation(entry, slotIdx);
			slot.SpawnRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);
			
			AllSlots.Add(slot);
			SpawnSlot(AllSlots.Last());
			
			if (AllSlots.Last().Object)
			{
				ObjectToSlotIndex.Add(AllSlots.Last().Object, globalSlotIdx);
			}
			
			++globalSlotIdx;
		}
	}
}

// ──────────────────────────────────────────────
//  스폰 / 비활성 / 재활성
// 
void APKObjectManager::SpawnSlot(FPKPoolSlot& Slot)
{
	if (!PoolEntries.IsValidIndex(Slot.EntryIndex)) return;
	const FPKPoolEntry& entry = PoolEntries[Slot.EntryIndex];
	if (!entry.PKObjectClass) return;
	
	UWorld* world = GetWorld();
	if (!world) return;
	
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	APKObject* newObj = world->SpawnActor<APKObject>(
		entry.PKObjectClass,
		Slot.SpawnLocation,
		Slot.SpawnRotation,
		spawnParams);
	
	if (newObj)
	{
		Slot.Object = newObj;
		Slot.bActive = true;
		Slot.bWaitingRespawn = false;
	}
}

void APKObjectManager::DeactivateSlot(FPKPoolSlot& Slot)
{
	if (!Slot.Object) return;
	
	// 액터 숨김, 충돌 틱 비활성화
	Slot.Object->SetActorHiddenInGame(true);
	Slot.Object->SetActorEnableCollision(false);
	Slot.Object->SetActorTickEnabled(false);
	
	// 물리 정지
	if (UPrimitiveComponent* prim = Slot.Object->FindComponentByClass<UPrimitiveComponent>())
	{
		prim->SetSimulatePhysics(false);
		prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
		prim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		
		Slot.bActive = false;
	}
}

void APKObjectManager::ReactivateSlot(FPKPoolSlot& Slot)
{
	if (!Slot.Object) return;
	
	// 원위치
	Slot.Object->SetActorLocation(Slot.SpawnLocation);
	Slot.Object->SetActorRotation(Slot.SpawnRotation);
	
	// 재활성화
	Slot.Object->SetActorHiddenInGame(false);
	Slot.Object->SetActorEnableCollision(true);
	Slot.Object->SetActorTickEnabled(true);
	
	// 물리 재설정
	if (UPrimitiveComponent* prim = Slot.Object->FindComponentByClass<UPrimitiveComponent>())
	{
		prim->SetSimulatePhysics(true);
		prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
		
		Slot.bActive = true;
		Slot.bWaitingRespawn = false;
	}
}

FVector APKObjectManager::CalcSpawnLocation(const FPKPoolEntry& Entry, int32 SlotIdx) const
{
	const FVector managerLoc = GetActorLocation();
	
	// 명시적 오프셋이 있으면 사용
	if (Entry.SpawnOffsetList.IsValidIndex(SlotIdx))
	{
		return managerLoc + Entry.SpawnOffsetList[SlotIdx];
	}
	
	// 없으면 랜덤 반경 내 배치
	const float angle = FMath::RandRange(0.f, 2.f * PI);
	const float radius = FMath::RandRange(0.f, Entry.RandomSpawnRadius);
	return managerLoc + FVector(FMath::Cos(angle) * radius, FMath::Sin(angle) * radius, 0.f);
}
