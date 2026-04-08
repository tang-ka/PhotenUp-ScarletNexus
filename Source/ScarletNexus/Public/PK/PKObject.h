// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PKInteractable.h"
#include "PKObject.generated.h"

class UDissolveComponent;
class APKObject;

// PK 던지기 충돌 시 호출되는 델리게이트 (PsychokinesisComponent에서 바인딩)
DECLARE_DELEGATE_FourParams(FOnPKObjectHit,
	APKObject*,             // 충돌한 PK 오브젝트
	AActor*,                // 충돌 대상 액터
	UPrimitiveComponent*,   // 충돌 대상 컴포넌트
	const FHitResult&);     // 충돌 결과

UENUM()
enum class EPKObjectState : uint8
{
	CanBePickedUp = 0, // 집기 전
	IsHeld,			   // 집힌 상태
	CoolDown,		   // 잡히고 떨어지는 상태 (이 때 다시 잡히면 안됨)
	IsUsed			   // 사용된 상태 (Thrown, Crumple, Ride)
};

UENUM()
enum class EPKObjectType : uint8
{
	Throwable = 0,	// 던질 수 있는
	Crumplable,		// 찌그러트릴 수 있는
	Rideable		// 탑승할 수 있는 (버스 등)
};

// PK : PsychokinesisObject (염력으로 집을 수 있는 물체)
UCLASS()
class SCARLETNEXUS_API APKObject : public AActor, public IPKInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APKObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 외형
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> StaticMeshComp;
	
	// <물리>
	// 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;
	// 질량
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Mass = 100.f;
	// 속력 (던져질 때 날아가는 경우)
	FVector Velocity = FVector::ZeroVector;
	
	// 피염력체 타입
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPKObjectType ObjectType = EPKObjectType::Throwable;
	
	// 피염력체 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPKObjectState ObjectState = EPKObjectState::CanBePickedUp;
	
	// 사용되었는지 체크
	bool bUsedObject = false;
	
	// 프로퍼티
	UFUNCTION(BlueprintCallable)
	EPKObjectType GetPKObjectType() { return ObjectType; }
	
	UFUNCTION(BlueprintCallable)
	EPKObjectState GetPKObjectState() { return ObjectState; }
	
	//**FX
	// Dissolve
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FX")
	UDissolveComponent* DissolveComp;
	
	// PK 인터페이스 구현
	virtual bool CanBePickeduped_Implementation() const override;
	virtual void OnPKPickuped_Implementation() override;
	virtual void OnPKReleased_Implementation() override;
	virtual void OnPKThrown_Implementation(const FVector& ThrowDir, float ThrowForce) override;
	virtual void OnPKThrownPS_Implementation(const FVector& ThrowDir, float ThrowForce) override;

	// === 충돌 델리게이트 (PsychokinesisComponent에서 바인딩) ===
	FOnPKObjectHit OnPKObjectHit;

private:
	// 충돌 감지 → 땅에 닿으면 CanBePickedUp으로 복귀
	UFUNCTION()
	void OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	              UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	              const FHitResult& Hit);

	// 비행 시간 초과 → 디졸브
	void OnFlightTimeout();

#pragma region Flight Timeout
	UPROPERTY(EditDefaultsOnly, Category="PK")
	float MaxFlightTime = 5.f;  // 던진 후 이 시간이 지나면 자동 디졸브 (초)

	FTimerHandle FlightTimerHandle;
#pragma endregion

#pragma region Throw Tilt
	UPROPERTY(EditDefaultsOnly, Category="PK|Tilt")
	float TiltAngleDeg = 45.f;    // 목표 틸트 각도 (도)

	UPROPERTY(EditDefaultsOnly, Category="PK|Tilt")
	float TiltDuration = 0.3f;    // 목표 각도까지 도달하는 시간 (초)

	bool bIsTilting = false;
	FQuat ThrowTiltTargetQuat = FQuat::Identity;
#pragma endregion
};
