// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PsychokinesisComponent.h"

#include "Camera/CameraComponent.h"
#include "Interface/PKInteractable.h"
#include "Player/PlayerCharacterBase.h"

UPsychokinesisComponent::UPsychokinesisComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPsychokinesisComponent::BeginPlay()
{
	Super::BeginPlay();

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
}

void UPsychokinesisComponent::SetPickedObject(AActor* NewPickedObject)
{
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
	
	bHolding = true;
	HoldStartLocation = PickedObject->GetActorLocation();
	HoldElapsedTime = 0.f;
	
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
	
	IPKInteractable::Execute_OnPKReleased(PickedObject.Get());
	
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
}

void UPsychokinesisComponent::Throw()
{	
	// 던지는 방향과 힘은 임시로 고정값 사용 (나중에 플레이어의 바라보는 방향과 조작량에 따라 다르게 설정)
	auto* Owner = Cast<APlayerCharacterBase>(GetOwner());
	FVector ThrowDirection{};
	
	if (!HasTarget())
	{
		ThrowDirection = Owner->GetCameraComp()->GetForwardVector();
	}
	else
	{
		ThrowDirection = ThrowTarget->GetActorLocation() - PickedObject->GetActorLocation();
	}
}

void UPsychokinesisComponent::StrongThrow()
{
	
}

