// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKComponent.h"

#include "Engine/OverlapResult.h"
#include "Interface/PKInteractable.h"
#include "PK/PKObject.h"
#include "Player/PlayerCharacterBase.h"


// Sets default values for this component's properties
UPKComponent::UPKComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPKComponent::BeginPlay()
{
	Super::BeginPlay();

	// 일정 시간 간격으로 트레이싱 설정
	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &UPKComponent::TraceTarget, TraceDelay, true);
}

void UPKComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}


// Called every frame
void UPKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (HeldObject)
	{
		UpdateHeldObjectPosition(DeltaTime);
	}
}

void UPKComponent::TraceTarget()
{
	// 초기화
	CurrentTarget = nullptr;
	// 주체
	AActor* owner = GetOwner();
	if (!owner)
	{
		UE_LOG(LogTemp, Error, TEXT("UPKComponent::TraceTarget: No owner"));
		return;
	}
	
	// 카메라 위치/방향
	FVector ownerLoc = owner->GetActorLocation();
	FVector forwardDir = GetOwnerForwardVector();
	
	// 반경 내 PKObject 프로파일 오버랩 탐색
	TArray<FOverlapResult> overlapResults;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owner);
	
	// 주위 PKObject 탐색
	GetWorld()->OverlapMultiByProfile(
		overlapResults,
		ownerLoc,
		FQuat::Identity,
		TEXT("PKObject"),
		FCollisionShape::MakeSphere(TraceDistance),
		params);
	
	APKObject* nearestPKObject = nullptr;
	float nearestDistance = FLT_MAX;
	
	for (auto& overlap : overlapResults)
	{
		AActor* actor = overlap.GetActor();
		if (!actor) continue;
		
		// PK 인터페이스 체크
		if (!actor->Implements<UPKInteractable>()) continue;
		
		// 이미 잡혀 있는 오브젝트 제외
		APKObject* pkObj = Cast<APKObject>(actor);
		if (!pkObj || !pkObj->Execute_CanBePickeduped(actor)) continue;
		
		// 시야각 필터링 (owner 전방 기준)
		FVector toActor = (actor->GetActorLocation() - ownerLoc).GetSafeNormal();
		float dotProduct = FVector::DotProduct(forwardDir, toActor);
		
		// 카메라 전방과 잡힌 액터 간의 벡터를 내적
		if (dotProduct < FOVDotThreshold) continue;
		
		// 최근접 오브젝트 선택
		float dist = FVector::Dist(ownerLoc, actor->GetActorLocation());
		if (dist < nearestDistance)
		{
			nearestDistance = dist;
			nearestPKObject = pkObj;
		}
	}
	
	CurrentTarget = nearestPKObject;
	if (CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PKComponent::TraceTarget] 최근접 PKObject: {0}"), CurrentTarget.GetFName());
	}
}

void UPKComponent::HoldTarget()
{
	if (HeldObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PKComponent::HoldTarget] 이미 들고 있는 오브젝트가 있음"));
		return;
	}
	
	if (!CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PKComponent::HoldTarget] CurrentTarget이 없음"));
		return;
	}
	
	if (!CurrentTarget->CanBePickeduped())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PKComponent::HoldTarget] 잡을 수 없는 상태"));
		return;
	}
	
	// PKObject 인터페이스 호출
	IPKInteractable::Execute_OnPKPickuped(CurrentTarget);
	CurrentTarget->ObjectState = EPKObjectState::IsHeld;
	
	HeldObject = CurrentTarget;
	HeldPrimitive = HeldObject->StaticMeshComp;
	CurrentTarget = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("[PKComponent::HoldTarget] 잡기 성공. HeldObject : %s"), *HeldObject->GetName());
}

void UPKComponent::ReleaseTarget()
{
	if (!HeldObject) return;
	
	IPKInteractable::Execute_OnPKReleased(HeldObject);
	HeldObject->ObjectState = EPKObjectState::CanBePickedUp;
	
	UE_LOG(LogTemp, Warning, TEXT("[PKComponent::HoldTarget] 잡기 해제. HeldObject : %s"), *HeldObject->GetName());
	
	HeldObject = nullptr;
	HeldPrimitive = nullptr;
}

void UPKComponent::UseHeldTarget(const FVector& ThrowDir, float ThrowForce)
{
	switch (HeldObject->ObjectType)
	{
		// 던지기 (자동차, 자전거, 블록...)
	case EPKObjectType::Throwable : 
		IPKInteractable::Execute_OnPKThrown(HeldObject, ThrowDir, ThrowForce);
		HeldObject->ObjectState = EPKObjectState::IsUsed;
		
		// 실제 임펄스 적용
		if (HeldPrimitive && HeldPrimitive->IsSimulatingPhysics())
		{
			// GT TODO : 이 공식은 직접 물리 구현 예정 
			HeldPrimitive->AddImpulse(ThrowDir.GetSafeNormal() * ThrowForce);
		}
		
		break;
		// 찌그러트리기 (드럼통...)
 	case EPKObjectType::Crumplable : 
		break;
		// 올라타기 (버스...)
	case EPKObjectType::Rideable : 
		break;
	}
	
	// 초기화
	HeldObject = nullptr;
	HeldPrimitive = nullptr;
}

FVector UPKComponent::GetOwnerForwardVector() const
{
	AActor* owner = GetOwner();
	if (!owner) return FVector::ForwardVector;
	
	// 플레이어인 경우 카메라 방향 사용
	if (APlayerCharacterBase* pawn = Cast<APlayerCharacterBase>(owner))
	{
		if (APlayerController* pc = Cast<APlayerController>(pawn->GetController()))
		{
			FVector camLoc;
			FRotator camRot;
			pc->GetPlayerViewPoint(camLoc, camRot);
			return camRot.Vector();
		}
	}
	
	// AI (파티 멤버) 경우 액터 전방 사용
	return owner->GetActorForwardVector();
}

void UPKComponent::UpdateHeldObjectPosition(float DeltaTime)
{
	if (!HeldPrimitive) return;
	
	FVector targetPos = GetHoldTargetLocation();
	FVector currPos = HeldPrimitive->GetComponentLocation();
	
	// 부드럽게 홀드 위치로 이동
	FVector newPos = FMath::VInterpTo(currPos, targetPos, DeltaTime, 10.f);
	HeldPrimitive->SetWorldLocation(newPos, false, nullptr, ETeleportType::TeleportPhysics);
}

FVector UPKComponent::GetHoldTargetLocation() const
{
	AActor* owner = GetOwner();
	if (!owner) return FVector::ZeroVector;
	
	// 오너 앞 HoldDistance + 약간 위
	return owner->GetActorLocation() + owner->GetActorForwardVector() * HoldDistance + FVector(0.f, 0.f, 80.f);
}
