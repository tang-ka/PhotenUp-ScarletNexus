// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Widget/Data/DamageWidgetData.h"
#include "PsychokinesisComponent.generated.h"


class APlayerCharacterBase;
class APKObject;
class UCameraShakeBase;

// PK 데미지 적용 시 브로드캐스트 (위젯 풀 등 외부에서 구독)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPKDamageDealt, const FDamageWidgetData&);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UPsychokinesisComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPsychokinesisComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Getters/Setters
	UFUNCTION(BlueprintCallable)
	bool HasPickedObject() const { return PickedObject.IsValid(); }
	
	UFUNCTION(BlueprintCallable)
	bool HasTarget() const { return ThrowTarget.IsValid(); }
	
	UFUNCTION(BlueprintCallable)
	void SetPickedObject(AActor* NewPickedObject);
	
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget) { ThrowTarget = NewTarget; }

	/** 기본공격 히트 확인 플래그 설정 (true 시 콤보 허용 시간 타이머 시작) */
	void SetBasicAttackHitConfirmed(bool bConfirmed);

	/** 기본공격 히트 확인 플래그를 읽고 즉시 소비(false로 리셋) */
	bool ConsumeBasicAttackHitConfirmed()
	{
		const bool bWas = bBasicAttackHitConfirmed;
		bBasicAttackHitConfirmed = false;
		return bWas;
	}
#pragma endregion
	
	UFUNCTION(BlueprintCallable)
	void StartHold();
	
	UFUNCTION(BlueprintCallable)
	void ReleaseHold();
	
	UFUNCTION(BlueprintCallable)
	void Throw();
	
	UFUNCTION(BlueprintCallable)
	void StrongThrow(); // 기본 공격을 맞췄을 경우 발동

	// PK 데미지 적용 시 브로드캐스트 (DamageAmountWidgetPoolComponent 등에서 구독)
	FOnPKDamageDealt OnPKDamageDealt;
	
private:
	FVector CalculateLaunchVelocity(const FVector& StartLocation, const FVector& TargetLocation, float Speed) const;

	/** HoldTime 만료 시 호출 — 일반 PK → Throw(), 콤보 PK → 베지어 리프트 시작 */
	void OnHoldComplete();

	/** StrongThrow 베지어 리프트 완료 후 실제 발사 */
	void ExecuteStrongThrowLaunch();

	/** PKObject 충돌 델리게이트 핸들러 — 데미지/카메라쉐이크/위젯 처리 */
	void HandlePKObjectHit(APKObject* HitObject, AActor* HitActor,
	                       UPrimitiveComponent* OtherComp, const FHitResult& Hit);

	/** HitStop 실행 */
	void TriggerPKHitStop(bool bStrong);

	/** HitStop 종료 (TimeDilation 복원) */
	void EndPKHitStop();
	
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> Me;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=PK, meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<AActor> PickedObject;
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	TObjectPtr<UAnimMontage> PKStartMontage;
	
	FName HoldStartSectionName = TEXT("Capture");
	FName ThrowStartSectionName = TEXT("Throw");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=PK, meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<AActor> ThrowTarget;
	
#pragma region Throw Properties
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float ThrowSpeed = 4000.f; // 던지는 힘
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float StrongThrowSpeed = 5000.f; // 강한 던지는 힘 (기본 공격을 맞췄을 때)
#pragma endregion
	
#pragma region Hold Properties
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float HoldTime = 1.f; // 이 시간이 지나면 던짐.
	
	FTimerHandle HoldTimerHandle;
	
	bool bHolding = false; // Hold 중인지 여부
	
	FVector HoldStartLocation;
	float HoldElapsedTime = 0.f; // Hold 시작 후 경과 시간
#pragma endregion 
	
#pragma region Floating Properties
	UPROPERTY(EditDefaultsOnly, Category="PK|Floating")
	float FloatingHeight = 150.f; // 집어서 띄워지는 높이 (플레이어로부터)
	
	UPROPERTY(EditDefaultsOnly, Category="PK|Floating")
	float FloatingTime = 1.f;
#pragma endregion

#pragma region Strong Throw Properties
	/** 기본공격이 적에게 적중했는지 여부 */
	bool bBasicAttackHitConfirmed = false;

	/** 콤보 허용 시간 (이 시간 안에 PK를 시작해야 StrongThrow로 전환) */
	UPROPERTY(EditDefaultsOnly, Category="PK|StrongThrow")
	float ComboWindowDuration = 2.f;

	/** 콤보 허용 시간 만료 타이머 */
	FTimerHandle BasicAttackHitConfirmedTimerHandle;
	
	/** 베지어 리프트 진행 중 여부 */
	bool bStrongThrowing = false;
	
	/** 베지어 시작점 (오브젝트 원래 위치, 고정) */
	FVector StrongThrowBezierP0;
	
	/** 베지어 리프트 경과 시간 */
	float StrongThrowElapsedTime = 0.f;
	
	/** 베지어 리프트 소요 시간 */
	UPROPERTY(EditDefaultsOnly, Category="PK|StrongThrow")
	float StrongThrowLiftDuration = 0.5f;
	
	/** 플레이어 머리 위 오프셋 (베지어 도착점) */
	UPROPERTY(EditDefaultsOnly, Category="PK|StrongThrow")
	FVector StrongThrowHeadOffset = FVector(0.f, 0.f, 200.f);
	
	/** 베지어 곡선 제어점의 추가 높이 */
	UPROPERTY(EditDefaultsOnly, Category="PK|StrongThrow")
	float StrongThrowCurveElevation = 150.f;

	FName StrongThrowSectionName = TEXT("Throw");
#pragma endregion

#pragma region PK Hit
	/** 일반 Throw 적중 시 데미지 */
	UPROPERTY(EditDefaultsOnly, Category="PK|Hit")
	int32 PKHitDamage = 87;

	/** StrongThrow 적중 시 데미지 */
	UPROPERTY(EditDefaultsOnly, Category="PK|Hit")
	int32 PKStrongThrowHitDamage = 124;

	/** 일반 Throw 적중 시 카메라 쉐이크 */
	UPROPERTY(EditDefaultsOnly, Category="PK|Hit")
	TSubclassOf<UCameraShakeBase> PKNormalHitCameraShakeClass;

	/** StrongThrow 적중 시 카메라 쉐이크 */
	UPROPERTY(EditDefaultsOnly, Category="PK|Hit")
	TSubclassOf<UCameraShakeBase> PKStrongHitCameraShakeClass;

	/** 현재 던지기가 StrongThrow인지 여부 (HandlePKObjectHit 내부에서 데미지 분기용) */
	bool bCurrentThrowIsStrong = false;
#pragma endregion

#pragma region PK HitStop
	/** 일반 Throw HitStop 시간 배율 */
	UPROPERTY(EditDefaultsOnly, Category="PK|HitStop")
	float PKNormalHitStopTimeDilation = 0.05f;

	/** 일반 Throw HitStop 실시간 지속 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category="PK|HitStop")
	float PKNormalHitStopDuration = 0.2f;

	/** StrongThrow HitStop 시간 배율 */
	UPROPERTY(EditDefaultsOnly, Category="PK|HitStop")
	float PKStrongHitStopTimeDilation = 0.02f;

	/** StrongThrow HitStop 실시간 지속 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category="PK|HitStop")
	float PKStrongHitStopDuration = 0.6f;

	FTimerHandle PKHitStopTimerHandle;
#pragma endregion
};
