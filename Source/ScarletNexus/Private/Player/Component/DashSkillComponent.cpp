// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/DashSkillComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UDashSkillComponent::UDashSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDashSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		CachedCameraComp = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	}
}

void UDashSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessDash(DeltaTime);
	AdjustLookForward();
}

void UDashSkillComponent::StartDash(FVector InDashDirection, bool bAdjustLook)
{
	if (!OwnerCharacter || !bCanDash || OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	DashDirection = InDashDirection;
	bNeedAdjustLookForward = bAdjustLook;

	bIsDashing = true;
	bCanDash = false;

	DashDirection.Normalize();
	DashDirection.Z = 0.f;

	DashVelocity = DashDirection * (DashDistance / DashDuration);
	DashTimeRemaining = DashDuration;

	OwnerCharacter->GetCharacterMovement()->Velocity.Z = 0;
}

void UDashSkillComponent::ProcessDash(float DeltaTime)
{
	if (!bIsDashing || !OwnerCharacter)
	{
		return;
	}

	DashTimeRemaining -= DeltaTime;

	if (DashTimeRemaining <= 0.f)
	{
		EndDash();
	}
	else
	{
		OwnerCharacter->GetCharacterMovement()->Velocity = DashVelocity;
	}
}

void UDashSkillComponent::EndDash()
{
	bIsDashing = false;

	if (OwnerCharacter)
	{
		OwnerCharacter->GetCharacterMovement()->Velocity *= DashDampingFactor;
	}

	GetWorld()->GetTimerManager().SetTimer(
		DashCooldownTimer,
		this,
		&UDashSkillComponent::ResetDash,
		DashCooldown,
		false
	);
}

void UDashSkillComponent::ResetDash()
{
	bIsDashing = false;
	bCanDash = true;
}

void UDashSkillComponent::AdjustLookForward()
{
	if (!bNeedAdjustLookForward || !OwnerCharacter || !CachedCameraComp)
	{
		return;
	}

	const FQuat CurQuat = OwnerCharacter->GetActorQuat();
	FVector CameraForwardXYPlane = CachedCameraComp->GetForwardVector();
	CameraForwardXYPlane.Z = 0.f;
	CameraForwardXYPlane.Normalize();
	
	const FQuat TargetQuat = CameraForwardXYPlane.ToOrientationQuat();
	const FQuat NewQuat = FQuat::Slerp(CurQuat, TargetQuat, 0.5f);
	
	OwnerCharacter->SetActorRotation(NewQuat);
	if (FQuat::ErrorAutoNormalize(NewQuat, TargetQuat) < 0.01f)
	{
		OwnerCharacter->SetActorRotation(TargetQuat);
		bNeedAdjustLookForward = false;
	}
}
