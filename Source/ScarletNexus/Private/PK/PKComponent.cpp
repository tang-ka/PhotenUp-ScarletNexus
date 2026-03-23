// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKComponent.h"

#include "Engine/OverlapResult.h"
#include "Interface/PKInteractable.h"
#include "PK/PKObject.h"


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
	
	APlayerController* pc = Cast<APlayerController>(Cast<APawn>(owner)->GetController());
	if (!pc) return;
	
	
	// 카메라 위치/방향
	FVector camLoc = FVector::ZeroVector;
	FRotator camRot = FRotator::ZeroRotator;
	pc->GetPlayerViewPoint(camLoc, camRot);
	FVector camForward = camRot.Vector();
	
	// 탐색 범위 : owner 기준 TraceDistance 까지
	FVector start = owner->GetActorLocation();
	FQuat rot = FQuat::Identity;
	//FVector end = start + rot.Vector() * TraceDistance;
	
	// 범위 내 오브젝트 전부 탐색
	TArray<FOverlapResult> overlapResults;
	FCollisionShape sphere = FCollisionShape::MakeSphere(TraceDistance);
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(owner);
	
	// 주위 PKObject 탐색
	FHitResult Hit;
	bool bHit = GetWorld()->OverlapMultiByProfile(
		overlapResults,
		start,
		rot,
		TEXT("PKObject"),
		sphere,
		traceParams);
	
	APKObject* nearestPKObject = nullptr;
	float nearestDistance = FLT_MAX;
	
	for (auto& overlap : overlapResults)
	{
		AActor* actor = overlap.GetActor();
		if (!actor) continue;
		
		// PK 인터페이스 체크
		if (!actor->Implements<UPKInteractable>()) continue;
		
		// 시야각 필터링 (카메라 기준 FOV 내에 있는지 체크)
		FVector toActor = (actor->GetActorLocation() - camLoc).GetSafeNormal();
		float dotProduct = FVector::DotProduct(camForward, toActor);
		
		// 카메라 전방과 잡힌 액터 간의 벡터를 내적 -> 0.7보다 크면 45도 이내
		if (dotProduct < 0.7f) continue;
		
		// 최근접 오브젝트 선택
		float dist = FVector::Dist(start, actor->GetActorLocation());
		if (dist < nearestDistance)
		{
			nearestDistance = dist;
			nearestPKObject = Cast<APKObject>(actor);
		}
	}
	
	CurrentTarget = nearestPKObject;
	UE_LOG(LogTemp, Warning, TEXT("최근접 PKObject: {0}"), CurrentTarget.GetFName());
}

void UPKComponent::HoldTarget()
{
}

void UPKComponent::ReleaseTarget()
{
}

void UPKComponent::UseHeldTarget(const FVector& ThrowDir, float ThrowForce)
{
}
