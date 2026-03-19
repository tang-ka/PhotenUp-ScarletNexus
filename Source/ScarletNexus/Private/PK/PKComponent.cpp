// Fill out your copyright notice in the Description page of Project Settings.


#include "PK/PKComponent.h"

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

	// ...
	
}


// Called every frame
void UPKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

APKObject* UPKComponent::TraceTarget() const
{
	// 주체
	AActor* owner = GetOwner();
	if (!owner)
	{
		UE_LOG(LogTemp, Error, TEXT("UPKComponent::TraceTarget: No owner"));
		return nullptr;
	}
	
	// 탐색 범위 : owner 기준 TraceDistance 까지
	FVector start = owner->GetActorLocation();
	FRotator rot = owner->GetActorRotation();
	FVector end = start + rot.Vector() * TraceDistance;
	
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(owner);
	
	// 주위 PKObject 탐색
	FHitResult Hit;
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, 
		start, 
		end,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		traceParams
		);
	
	if (!bHit) return nullptr;
	
	AActor* hitActor = Hit.GetActor();
	if (!hitActor) return nullptr;
	if (!hitActor->GetClass()->ImplementsInterface(UPKInteractable::StaticClass())) return nullptr;
	
	APKObject* hitPKObject = Cast<APKObject>(hitActor);
	if (!hitPKObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UPKComponent::TraceTarget: HitActor의 PKObject 캐스팅에 실패했다."))
		return nullptr;
	}
	return hitPKObject;
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
