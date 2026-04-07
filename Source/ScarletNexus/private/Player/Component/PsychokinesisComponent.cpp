// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PsychokinesisComponent.h"

#include "ScarletNexus.h"
#include "Camera/CameraComponent.h"
#include "Interface/PKInteractable.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Component/PlayerPerceptionComponent.h"

UPsychokinesisComponent::UPsychokinesisComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPsychokinesisComponent::BeginPlay()
{
	Super::BeginPlay();

	Me = Cast<APlayerCharacterBase>(GetOwner());
}

void UPsychokinesisComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHolding && HasPickedObject())
	{
		// FloatingTime 동안 FloatingHeight만큼 위로 올림
		HoldElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(HoldElapsedTime / FloatingTime, 0.f, 1.f);
		const FVector TargetLocation = HoldStartLocation + FVector(0.f, 0.f, FloatingHeight);
		const FVector NewLocation = FMath::Lerp(HoldStartLocation, TargetLocation, Alpha);
		PickedObject->SetActorLocation(NewLocation);
	}

	if (bThrowing && HasPickedObject())
	{
		const FVector CurrentLocation = PickedObject->GetActorLocation();
		const FVector NewLocation = FMath::VInterpConstantTo(
			CurrentLocation, ThrowTargetLocation, DeltaTime, ThrowSpeed);

		PickedObject->SetActorLocation(NewLocation);

		// 타겟까지 남은 거리가 ThrowSpeed * DeltaTime 이하 → 도달한 것으로 판단하고 정지
		const float DistRemaining = FVector::Dist(NewLocation, ThrowTargetLocation);
		if (DistRemaining <= ThrowSpeed * DeltaTime + 1.f)
		{
			bThrowing = false;
			// PickedObject = nullptr; // 던지기 완료 → 참조 해제
		}
	}
}

void UPsychokinesisComponent::SetPickedObject(AActor* NewPickedObject)
{
	// TODO 상혁 확인 : 파티 캐릭이 선점했을 때 Null로 들어옴
	if (!IsValid(NewPickedObject))
	{
		return;
	}
	
	if (NewPickedObject->GetClass()->ImplementsInterface(UPKInteractable::StaticClass()))
	{
		PickedObject = NewPickedObject;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to set a non-PKInteractable object as PickedObject"));
	}
}

void UPsychokinesisComponent::StartHold()
{
	if (!HasPickedObject())
	{
		return;
	}
	
	// if (!IPKInteractable::Execute_CanBePickeduped(PickedObject.Get()))
	// {
	// 	return;
	// }

	// 이전 던지기 상태가 남아있을 경우 초기화
	// bThrowing = false;
	bHolding = true;
	HoldStartLocation = PickedObject->GetActorLocation();
	HoldElapsedTime = 0.f;
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(false);
	
	IPKInteractable::Execute_OnPKPickuped(PickedObject.Get());
	
	GetWorld()->GetTimerManager().SetTimer(
		HoldTimerHandle,
		this,
		&UPsychokinesisComponent::Throw,
		HoldTime,
		false
	);
}

void UPsychokinesisComponent::ReleaseHold()
{
	bHolding = false;
	// bThrowing = false;

	// ClearTimer는 PickedObject 유효 여부와 무관하게 항상 실행해야 함
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(true);
	
	// TODO 상혁 확인 : 파티 캐릭이 선점했을 때 Null로 들어옴
	if (!IsValid(PickedObject.Get())) return;
	
	IPKInteractable::Execute_OnPKReleased(PickedObject.Get());
	PickedObject = nullptr;
}

void UPsychokinesisComponent::Throw()
{	
	if (!HasPickedObject()) return;

	// 던지는 방향 결정
	if (!HasTarget())
	{
		ThrowDirection = Me->GetCameraComp()->GetForwardVector();
		// GetForwardVector()는 크기 1인 단위벡터 → Z에 직접 50 대입 금지
		// 약간 위쪽을 향하도록 Z 성분을 보정 후 다시 정규화
		ThrowDirection += FVector(0.f, 0.f, 0.3f);
		ThrowDirection.Normalize();
		ThrowTargetLocation = PickedObject->GetActorLocation() + ThrowDirection * 3000.f;
	}
	else
	{
		FVector Origin;
		FVector BoxExtent;
		ThrowTarget->GetActorBounds(true, Origin, BoxExtent);
		const float ZOffset = BoxExtent.Z / 4;
		
		ThrowTargetLocation = ThrowTarget->GetActorLocation();
		ThrowTargetLocation.Z += ZOffset; // 타겟의 중심이 아닌 위쪽을 향하도록 Z 오프셋 추가
		ThrowDirection = (ThrowTargetLocation - PickedObject->GetActorLocation()).GetSafeNormal();
	}

	IPKInteractable::Execute_OnPKThrown(PickedObject.Get(), ThrowDirection, ThrowSpeed);

	bThrowing = true;
	bHolding  = false;
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(true);
}

void UPsychokinesisComponent::StrongThrow()
{
	
}

