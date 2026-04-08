// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PsychokinesisComponent.h"

#include "ScarletNexus.h"
#include "Camera/CameraComponent.h"
#include "Interface/PKInteractable.h"
#include "Player/PlayerCharacterBase.h"
#include "Player/Animation/KasaneAnimInstance.h"
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
		// FloatingTime 동안 FloatingHeight만큼 위로 올림 (제자리 띄우기)
		HoldElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(HoldElapsedTime / FloatingTime, 0.f, 1.f);

		// EaseOutCubic: 처음에 빠르게 튀어오르다가 끝에서 부드럽게 감속
		const float EasedAlpha = 1.f - FMath::Pow(1.f - Alpha, 3.f);

		const FVector TargetLocation = HoldStartLocation + FVector(0.f, 0.f, FloatingHeight);
		const FVector NewLocation = FMath::Lerp(HoldStartLocation, TargetLocation, EasedAlpha);
		PickedObject->SetActorLocation(NewLocation);
	}
	else if (bStrongThrowing && HasPickedObject())
	{
		// ── 베지어 곡선으로 오브젝트를 플레이어 머리 위로 끌어올리기 ──
		StrongThrowElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(StrongThrowElapsedTime / StrongThrowLiftDuration, 0.f, 1.f);
		const float T = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

		// P0: 시작점 (고정), P2: 플레이어 머리 위 (매 틱 갱신), P1: 중간 제어점
		const FVector P0 = StrongThrowBezierP0;
		const FVector P2 = Me->GetActorLocation() + StrongThrowHeadOffset;
		const FVector P1 = (P0 + P2) * 0.5f + FVector(0.f, 0.f, StrongThrowCurveElevation);

		// Quadratic Bezier: B(t) = (1-t)²·P0 + 2(1-t)t·P1 + t²·P2
		const float OneMinusT = 1.f - T;
		const FVector Pos = (OneMinusT * OneMinusT) * P0
		                   + 2.f * OneMinusT * T * P1
		                   + (T * T) * P2;

		PickedObject->SetActorLocation(Pos);

		if (Alpha >= 1.f)
		{
			// 리프트 완료 → 자동 발사
			ExecuteStrongThrowLaunch();
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
		PRINTLOG_SH(TEXT("[Psychokinesis] Hold 시작 실패: PickedObject 없음"));
		return;
	}

	if (!IPKInteractable::Execute_CanBePickeduped(PickedObject.Get()))
	{
		PRINTLOG_SH(TEXT("[Psychokinesis] Hold 시작 실패: PickedObject가 집을 수 없는 상태입니다."));
		return;
	}

	// 이전 던지기 상태가 남아있을 경우 초기화
	bHolding = true;
	HoldStartLocation = PickedObject->GetActorLocation();
	HoldElapsedTime = 0.f;
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(false);

	IPKInteractable::Execute_OnPKPickuped(PickedObject.Get());

	// HoldTime 후 OnHoldComplete에서 일반/콤보 분기
	GetWorld()->GetTimerManager().SetTimer(
		HoldTimerHandle,
		this,
		&UPsychokinesisComponent::OnHoldComplete,
		HoldTime,
		false
	);

	// AnimInstance에 Hold 상태 전달 (루프 포즈용)
	Me->PlayAnimMontage(PKStartMontage, 1.f, HoldStartSectionName);
	if (auto* Anim = Cast<UKasaneAnimInstance>(Me->GetMesh()->GetAnimInstance()))
	{
		Anim->SetIsPKHolding(true);
	}
}

void UPsychokinesisComponent::ReleaseHold()
{
	bHolding = false;
	bStrongThrowing = false; // StrongThrow 중 해제될 경우 정리라

	// ClearTimer는 PickedObject 유효 여부와 무관하게 항상 실행해야 함
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(true);

	// TODO 상혁 확인 : 파티 캐릭이 선점했을 때 Null로 들어옴
	if (!IsValid(PickedObject.Get())) return;

	IPKInteractable::Execute_OnPKReleased(PickedObject.Get());
	PickedObject = nullptr;
	
	if (auto* Anim = Cast<UKasaneAnimInstance>(Me->GetMesh()->GetAnimInstance()))
	{
		Anim->SetIsPKHolding(false);
	}
}

void UPsychokinesisComponent::Throw()
{
	if (!HasPickedObject())
	{
		return;
	}

	FVector ThrowDirection;
	FVector ThrowTargetLocation;

	// 던지는 방향 결정
	if (!HasTarget())
	{
		ThrowDirection = Me->GetCameraComp()->GetForwardVector();
		// 약간 위쪽을 향하도록 Z 성분을 보정 후 다시 정규화
		ThrowDirection += FVector(0.f, 0.f, 0.4f);
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

	// 중력 보정 velocity 계산
	FVector LaunchVelocity = CalculateLaunchVelocity(
		PickedObject->GetActorLocation(),
		ThrowTargetLocation, ThrowSpeed);

	IPKInteractable::Execute_OnPKThrown(PickedObject.Get(), ThrowDirection, ThrowSpeed);

	bHolding = false;
	PickedObject = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(true);
	
	Me->PlayAnimMontage(PKStartMontage, 1.f, ThrowStartSectionName);
	if (auto* Anim = Cast<UKasaneAnimInstance>(Me->GetMesh()->GetAnimInstance()))
	{
		Anim->SetIsPKHolding(false);
	}
}

void UPsychokinesisComponent::OnHoldComplete()
{
	if (!HasPickedObject())
	{
		bHolding = false;
		return;
	}

	if (ConsumeBasicAttackHitConfirmed())
	{
		// ── 콤보 염동력: Hold → 베지어 리프트 → StrongThrow 발사 ──
		PRINTLOG_SH(TEXT("[Psychokinesis] 콤보 PK: 베지어 리프트로 전환"));

		bHolding = false;
		bStrongThrowing = true;
		StrongThrowElapsedTime = 0.f;
		StrongThrowBezierP0 = PickedObject->GetActorLocation();
	}
	else
	{
		// ── 일반 염동력: 바로 던지기 ──
		Throw();
	}
}

void UPsychokinesisComponent::StrongThrow()
{
	// 외부에서 직접 호출용 (StartHold를 거치지 않고 즉시 베지어 리프트)
	if (!HasPickedObject())
	{
		PRINTLOG_SH(TEXT("[Psychokinesis] StrongThrow 시작 실패: PickedObject 없음"));
		return;
	}

	if (!IPKInteractable::Execute_CanBePickeduped(PickedObject.Get()))
	{
		PRINTLOG_SH(TEXT("[Psychokinesis] StrongThrow 시작 실패: PickedObject가 집을 수 없는 상태입니다."));
		return;
	}

	// 이전 Hold 상태가 남아있으면 정리
	bHolding = false;
	GetWorld()->GetTimerManager().ClearTimer(HoldTimerHandle);

	// 베지어 리프트 시작
	bStrongThrowing = true;
	StrongThrowElapsedTime = 0.f;
	StrongThrowBezierP0 = PickedObject->GetActorLocation();

	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(false);

	IPKInteractable::Execute_OnPKPickuped(PickedObject.Get());

	// PK 몽타주 재생
	Me->PlayAnimMontage(PKStartMontage, 1.f, HoldStartSectionName);
	if (auto* Anim = Cast<UKasaneAnimInstance>(Me->GetMesh()->GetAnimInstance()))
	{
		Anim->SetIsPKHolding(true);
	}

	PRINTLOG_SH(TEXT("[Psychokinesis] StrongThrow 베지어 리프트 시작"));
}

void UPsychokinesisComponent::ExecuteStrongThrowLaunch()
{
	if (!HasPickedObject())
	{
		bStrongThrowing = false;
		return;
	}

	FVector ThrowDirection;
	FVector ThrowTargetLocation;

	// 던지는 방향 결정
	if (!HasTarget())
	{
		ThrowDirection = Me->GetCameraComp()->GetForwardVector();
		ThrowDirection += FVector(0.f, 0.f, 0.4f);
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
		ThrowTargetLocation.Z += ZOffset;
		ThrowDirection = (ThrowTargetLocation - PickedObject->GetActorLocation()).GetSafeNormal();
	}

	// StrongThrowSpeed로 발사
	IPKInteractable::Execute_OnPKThrown(PickedObject.Get(), ThrowDirection, StrongThrowSpeed);

	bStrongThrowing = false;
	PickedObject = nullptr;
	Me->GetPerceptionComp()->SetActivePsychokinesisTargetUpdate(true);

	// Throw 몽타주 재생
	Me->PlayAnimMontage(PKStartMontage, 1.f, StrongThrowSectionName);
	if (auto* Anim = Cast<UKasaneAnimInstance>(Me->GetMesh()->GetAnimInstance()))
	{
		Anim->SetIsPKHolding(false);
	}

	PRINTLOG_SH(TEXT("[Psychokinesis] StrongThrow 발사 완료 (Speed: %.0f)"), StrongThrowSpeed);
}

FVector UPsychokinesisComponent::CalculateLaunchVelocity(const FVector& StartLocation, const FVector& TargetLocation,
                                                         float Speed) const
{
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
	float Distance = FVector::Dist(StartLocation, TargetLocation);

	// 비행 시간 추정
	float FlightTime = Distance / Speed;

	// 중력 보정 : vz = -0.5 * g * t (UE 기본 중력 음수)
	float GravityZ = GetWorld()->GetGravityZ(); // 보통 -980.f
	float ZCompensation = -0.5f * GravityZ * FlightTime;

	FVector Velocity = Direction * Speed;
	Velocity.Z += ZCompensation;

	return Velocity;
}
