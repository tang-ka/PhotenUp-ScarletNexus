// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKObjectManager.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

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
	DeactivateTimers.Empty();
	
	int32 globalSlotIdx = 0;
	
	// 1패스 : 일단 전부 스폰하고 실제 바운드 기록
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
	
	// 2패스 : 실제 바운드 기반으로 겹치지 않는 위치 재배치
	for (int32 i = 0; i < AllSlots.Num(); ++i)
	{
		FPKPoolSlot slot = AllSlots[i];
		if (!slot.Object) continue;
		if (!PoolEntries.IsValidIndex(slot.EntryIndex)) continue;
		
		const FPKPoolEntry& entry = PoolEntries[slot.EntryIndex];
		
		// 이 오브젝트의 XY 반경 (긴 쪽 기준)
		const float myRadius = FMath::Max(slot.BoundsExtent.X, slot.BoundsExtent.Y);
		
		// 명시적 오프셋
		if (entry.SpawnOffsetList.IsValidIndex(slot.SlotIndex))
		{
			FVector finalLoc = GetActorLocation() + entry.SpawnOffsetList[slot.SlotIndex];
			SnapToGround(finalLoc, entry.GroundTraceDistance);
			finalLoc.Z += slot.BoundsExtent.Z;
			slot.Object->SetActorLocation(finalLoc);
			slot.SpawnLocation = slot.Object->GetActorLocation();
			continue;
		}
		
		// 랜덤 배치 : 이미 배치된 슬롯들의 실제 바운드와 비교
		const int32 maxAttempts = 50;
		FVector bestLoc = GetActorLocation();
		
		for (int32 attempt = 0; attempt < maxAttempts; ++attempt)
		{
			const float angle = FMath::RandRange(0.f, 2.0f * PI);
			const float radius = FMath::RandRange(myRadius * 2.f, entry.RandomSpawnRadius);
			FVector candidate = GetActorLocation() + FVector(FMath::Cos(angle) * radius, FMath::Sin(angle) * radius, 0.f);
			
			bool bTooClose = false;
			for (int32 j = 0; j < i; ++j)
			{
				const FPKPoolSlot& other = AllSlots[j];
				if (!other.Object) continue;
				
				// 양쪽의 긴 쪽 extent 합산 + 여유값
				const float otherRadius = FMath::Max(other.BoundsExtent.X + slot.BoundsExtent.Y);
				const float requiredDist = myRadius + otherRadius + entry.MinSpacing;
				
				if (FVector::Dist2D(candidate, other.SpawnLocation) < requiredDist)
				{
					bTooClose = true;
					break;
				}
			}
			
			if (!bTooClose)
			{
				bestLoc = candidate;
				break;
			}
			
			// 마지막 시도면 강제 배치
			if (attempt == maxAttempts - 1)
			{
				const float fallbackRadius = entry.RandomSpawnRadius + myRadius * 2.f * (slot.SlotIndex + 1);
				bestLoc = GetActorLocation() + FVector(FMath::Cos(angle) * fallbackRadius, FMath::Sin(angle) * fallbackRadius, 0.f);
			}
		}
		
		// 지면 스냅 + Z extent로 정확히 지면 위에 배치
		SnapToGround(bestLoc, entry.GroundTraceDistance);
		bestLoc.Z += slot.BoundsExtent.Z;
		slot.Object->SetActorLocation(bestLoc);
		slot.SpawnLocation = slot.Object->GetActorLocation();
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
	
	// 스폰
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
		// 바운드 절반 높이만큼 올려서 땅 위에 얹기
		FVector origin, boxExtent;
		newObj->GetActorBounds(false, origin, boxExtent);
		
		Slot.BoundsExtent = boxExtent;
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
	
	// 1패스 임시 위치 : 겹침 체크는 2패스에서 실제 바운드로 처리
	return managerLoc;
}

void APKObjectManager::SnapToGround(FVector& Location, float TraceDistance) const
{
}
